/*
    Copyright (c) 2007-2016 Contributors as noted in the AUTHORS file

    This file is part of libzmq, the ZeroMQ core engine in C++.

    libzmq is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    As a special exception, the Contributors give you permission to link
    this library with independent modules to produce an executable,
    regardless of the license terms of these independent modules, and to
    copy and distribute the resulting executable under terms of your choice,
    provided that you also meet, for each linked independent module, the
    terms and conditions of the license of that module. An independent
    module is a module which is not derived from or based on this library.
    If you modify this library, you must extend this exception to your
    version of the library.

    libzmq is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __ZMQ_YPIPE_HPP_INCLUDED__
#define __ZMQ_YPIPE_HPP_INCLUDED__

//  This class encapsulates several atomic operations on pointers.

template <typename T> class atomic_ptr_t
{
public:
	//  Initialise atomic pointer
	inline atomic_ptr_t() { ptr = NULL; }

	//  Destroy atomic pointer
	inline ~atomic_ptr_t() {}

	//  Set value of atomic pointer in a non-threadsafe way
	//  Use this function only when you are sure that at most one
	//  thread is accessing the pointer at the moment.
	inline void set(T *ptr_) { this->ptr = ptr_; }

	//  Perform atomic 'exchange pointers' operation. Pointer is set
	//  to the 'val' value. Old value is returned.
	inline T *xchg(T *val_)
	{
		return ptr.exchange(val_, std::memory_order_acq_rel);
	}

	//  Perform atomic 'compare and swap' operation on the pointer.
	//  The pointer is compared to 'cmp' argument and if they are
	//  equal, its value is set to 'val'. Old value of the pointer
	//  is returned.
	inline T *cas(T *cmp_, T *val_)
	{
		ptr.compare_exchange_strong(cmp_, val_, std::memory_order_acq_rel);
		return cmp_;
	}

private:

	std::atomic<T *> ptr;
};

// ypipe_base abstracts ypipe and ypipe_conflate specific
// classes, one is selected according to a the conflate
// socket option

template <typename T> class ypipe_base_t
{
public:
	virtual ~ypipe_base_t() {}
	virtual void write(const T &value_, bool incomplete_) = 0;
	virtual bool unwrite(T *value_) = 0;
	virtual bool flush() = 0;
	virtual bool check_read() = 0;
	virtual bool read(T *value_) = 0;
	virtual bool probe(bool(*fn) (const T &)) = 0;
};

//  yqueue is an efficient queue implementation. The main goal is
//  to minimise number of allocations/deallocations needed. Thus yqueue
//  allocates/deallocates elements in batches of N.
//
//  yqueue allows one thread to use push/back function and another one
//  to use pop/front functions. However, user must ensure that there's no
//  pop on the empty queue and that both threads don't access the same
//  element in unsynchronised manner.
//
//  T is the type of the object in the queue.
//  N is granularity of the queue (how many pushes have to be done till
//  actual memory allocation is required).
#ifdef HAVE_POSIX_MEMALIGN
// ALIGN is the memory alignment size to use in the case where we have
// posix_memalign available. Default value is 64, this alignment will
// prevent two queue chunks from occupying the same CPU cache line on
// architectures where cache lines are <= 64 bytes (e.g. most things
// except POWER).
template <typename T, int N, size_t ALIGN = 64> class yqueue_t
#else
template <typename T, int N> class yqueue_t
#endif
{
public:
	//  Create the queue.
	inline yqueue_t()
	{
		begin_chunk = allocate_chunk();
		alloc_assert(begin_chunk);
		begin_pos = 0;
		back_chunk = NULL;
		back_pos = 0;
		end_chunk = begin_chunk;
		end_pos = 0;
	}

	//  Destroy the queue.
	inline ~yqueue_t()
	{
		while (true) {
			if (begin_chunk == end_chunk) {
				free(begin_chunk);
				break;
			}
			chunk_t *o = begin_chunk;
			begin_chunk = begin_chunk->next;
			free(o);
		}

		chunk_t *sc = spare_chunk.xchg(NULL);
		free(sc);
	}

	//  Returns reference to the front element of the queue.
	//  If the queue is empty, behaviour is undefined.
	inline T &front() { return begin_chunk->values[begin_pos]; }

	//  Returns reference to the back element of the queue.
	//  If the queue is empty, behaviour is undefined.
	inline T &back() { return back_chunk->values[back_pos]; }

	//  Adds an element to the back end of the queue.
	inline void push()
	{
		back_chunk = end_chunk;
		back_pos = end_pos;

		if (++end_pos != N)
			return;

		chunk_t *sc = spare_chunk.xchg(NULL);
		if (sc) {
			end_chunk->next = sc;
			sc->prev = end_chunk;
		}
		else {
			end_chunk->next = allocate_chunk();
			alloc_assert(end_chunk->next);
			end_chunk->next->prev = end_chunk;
		}
		end_chunk = end_chunk->next;
		end_pos = 0;
	}

	//  Removes element from the back end of the queue. In other words
	//  it rollbacks last push to the queue. Take care: Caller is
	//  responsible for destroying the object being unpushed.
	//  The caller must also guarantee that the queue isn't empty when
	//  unpush is called. It cannot be done automatically as the read
	//  side of the queue can be managed by different, completely
	//  unsynchronised thread.
	inline void unpush()
	{
		//  First, move 'back' one position backwards.
		if (back_pos)
			--back_pos;
		else {
			back_pos = N - 1;
			back_chunk = back_chunk->prev;
		}

		//  Now, move 'end' position backwards. Note that obsolete end chunk
		//  is not used as a spare chunk. The analysis shows that doing so
		//  would require free and atomic operation per chunk deallocated
		//  instead of a simple free.
		if (end_pos)
			--end_pos;
		else {
			end_pos = N - 1;
			end_chunk = end_chunk->prev;
			free(end_chunk->next);
			end_chunk->next = NULL;
		}
	}

	//  Removes an element from the front end of the queue.
	inline void pop()
	{
		if (++begin_pos == N) {
			chunk_t *o = begin_chunk;
			begin_chunk = begin_chunk->next;
			begin_chunk->prev = NULL;
			begin_pos = 0;

			//  'o' has been more recently used than spare_chunk,
			//  so for cache reasons we'll get rid of the spare and
			//  use 'o' as the spare.
			chunk_t *cs = spare_chunk.xchg(o);
			free(cs);
		}
	}

private:
	//  Individual memory chunk to hold N elements.
	struct chunk_t
	{
		T values[N];
		chunk_t *prev;
		chunk_t *next;
	};

	inline chunk_t *allocate_chunk()
	{
#ifdef HAVE_POSIX_MEMALIGN
		void *pv;
		if (posix_memalign(&pv, ALIGN, sizeof(chunk_t)) == 0)
			return (chunk_t *)pv;
		return NULL;
#else
		return (chunk_t *)malloc(sizeof(chunk_t));
#endif
	}

	//  Back position may point to invalid memory if the queue is empty,
	//  while begin & end positions are always valid. Begin position is
	//  accessed exclusively be queue reader (front/pop), while back and
	//  end positions are accessed exclusively by queue writer (back/push).
	chunk_t *begin_chunk;
	int begin_pos;
	chunk_t *back_chunk;
	int back_pos;
	chunk_t *end_chunk;
	int end_pos;

	//  People are likely to produce and consume at similar rates.  In
	//  this scenario holding onto the most recently freed chunk saves
	//  us from having to call malloc/free.
	atomic_ptr_t<chunk_t> spare_chunk;

	//  Disable copying of yqueue.
	yqueue_t(const yqueue_t &);
	const yqueue_t &operator= (const yqueue_t &);
};

//  Lock-free queue implementation.
//  Only a single thread can read from the pipe at any specific moment.
//  Only a single thread can write to the pipe at any specific moment.
//  T is the type of the object in the queue.
//  N is granularity of the pipe, i.e. how many items are needed to
//  perform next memory allocation.

template <typename T, int N> class ypipe_t : public ypipe_base_t<T>
{
  public:
    //  Initialises the pipe.
    inline ypipe_t ()
    {
        //  Insert terminator element into the queue.
        queue.push ();

        //  Let all the pointers to point to the terminator.
        //  (unless pipe is dead, in which case c is set to NULL).
        r = w = f = &queue.back ();
        c.set (&queue.back ());
    }

    //  The destructor doesn't have to be virtual. It is made virtual
    //  just to keep ICC and code checking tools from complaining.
    inline virtual ~ypipe_t () {}

        //  Following function (write) deliberately copies uninitialised data
        //  when used with zmq_msg. Initialising the VSM body for
        //  non-VSM messages won't be good for performance.

    //  Write an item to the pipe.  Don't flush it yet. If incomplete is
    //  set to true the item is assumed to be continued by items
    //  subsequently written to the pipe. Incomplete items are never
    //  flushed down the stream.
    inline void write (const T &value_, bool incomplete_)
    {
        //  Place the value to the queue, add new terminator element.
        queue.back () = value_;
        queue.push ();

        //  Move the "flush up to here" poiter.
        if (!incomplete_)
            f = &queue.back ();
    }

    //  Pop an incomplete item from the pipe. Returns true if such
    //  item exists, false otherwise.
    inline bool unwrite (T *value_)
    {
        if (f == &queue.back ())
            return false;
        queue.unpush ();
        *value_ = queue.back ();
        return true;
    }

    //  Flush all the completed items into the pipe. Returns false if
    //  the reader thread is sleeping. In that case, caller is obliged to
    //  wake the reader up before using the pipe again.
    inline bool flush ()
    {
        //  If there are no un-flushed items, do nothing.
        if (w == f)
            return true;

        //  Try to set 'c' to 'f'.
        if (c.cas (w, f) != w) {
            //  Compare-and-swap was unseccessful because 'c' is NULL.
            //  This means that the reader is asleep. Therefore we don't
            //  care about thread-safeness and update c in non-atomic
            //  manner. We'll return false to let the caller know
            //  that reader is sleeping.
            c.set (f);
            w = f;
            return false;
        }

        //  Reader is alive. Nothing special to do now. Just move
        //  the 'first un-flushed item' pointer to 'f'.
        w = f;
        return true;
    }

    //  Check whether item is available for reading.
    inline bool check_read ()
    {
        //  Was the value prefetched already? If so, return.
        if (&queue.front () != r && r)
            return true;

        //  There's no prefetched value, so let us prefetch more values.
        //  Prefetching is to simply retrieve the
        //  pointer from c in atomic fashion. If there are no
        //  items to prefetch, set c to NULL (using compare-and-swap).
        r = c.cas (&queue.front (), NULL);

        //  If there are no elements prefetched, exit.
        //  During pipe's lifetime r should never be NULL, however,
        //  it can happen during pipe shutdown when items
        //  are being deallocated.
        if (&queue.front () == r || !r)
            return false;

        //  There was at least one value prefetched.
        return true;
    }

    //  Reads an item from the pipe. Returns false if there is no value.
    //  available.
    inline bool read (T *value_)
    {
        //  Try to prefetch a value.
        if (!check_read ())
            return false;

        //  There was at least one value prefetched.
        //  Return it to the caller.
        *value_ = queue.front ();
        queue.pop ();
        return true;
    }

    //  Applies the function fn to the first elemenent in the pipe
    //  and returns the value returned by the fn.
    //  The pipe mustn't be empty or the function crashes.
    inline bool probe (bool (*fn) (const T &))
    {
        bool rc = check_read ();
        zmq_assert (rc);

        return (*fn) (queue.front ());
    }

  protected:
    //  Allocation-efficient queue to store pipe items.
    //  Front of the queue points to the first prefetched item, back of
    //  the pipe points to last un-flushed item. Front is used only by
    //  reader thread, while back is used only by writer thread.
    yqueue_t<T, N> queue;

    //  Points to the first un-flushed item. This variable is used
    //  exclusively by writer thread.
    T *w;

    //  Points to the first un-prefetched item. This variable is used
    //  exclusively by reader thread.
    T *r;

    //  Points to the first item to be flushed in the future.
    T *f;

    //  The single point of contention between writer and reader thread.
    //  Points past the last flushed item. If it is NULL,
    //  reader is asleep. This pointer should be always accessed using
    //  atomic operations.
    atomic_ptr_t<T> c;

    //  Disable copying of ypipe object.
    ypipe_t (const ypipe_t &);
    const ypipe_t &operator= (const ypipe_t &);
};

#endif
