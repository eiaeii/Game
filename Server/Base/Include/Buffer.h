#ifndef _BUFFER_H
#define _BUFFER_H

#include <assert.h>
#include <memory.h>
#include <string>

/**
buffer base class
*/
class basic_buffer
{
public:
	// buffer state
	enum buf_state
	{
		_good = 0,	// all right!
		_eof = 1,	// range error, less begin of buffer or more end of buffer
		_fail = 2,	// operator error, include invalid parameter,logic error, memory error,etc
	};

protected:
	unsigned char* _data;	// buffer
	size_t	_cur;	// cursor
	size_t	_cap;	// capacity
	int		_state;	// buffer state

public:
	basic_buffer()
		: _data(0), _cap(0), _cur(0), _state(_good)
	{
	}

	basic_buffer(unsigned char* buf, size_t maxsize)
		: _data(buf), _cap(maxsize), _cur(0), _state(_good)
	{
		assert(buf != 0 && maxsize > 0);
	}

	~basic_buffer() { }

	//---basic member access
	// get data pointer
	unsigned char* data() const { return _data; }

	// set data pointer
	void set_data(unsigned char* d) { _data = d; }

	// get current size
	size_t size() const { return _cur; }

	// set current size
	//void size(size_t cur_)	{ _cur = cur_; }

	// get capacity
	size_t cap() const { return _cap; }
	/// set capacity
	void cap(size_t cap_) { _cap = cap_; }

	// state
	int state() const { return _state; }

	//---pointer helper
	// begin pointer of data
	unsigned char* begin() const { return _data; }

	// current data pointer
	unsigned char* current()	const { return _data + _cur; }

	// end pointer of data
	unsigned char* end()	const { return _data + _cap; }

	// data pointer of specific position
	unsigned char* pos(size_t n) const { if (n <= _cap) return _data + n; return 0; }

	//---size helper
	// remain space
	size_t space() const { return right(); }

	//---state helper
	// is not good?
	bool operator !() const { return !good(); }

	// is good?
	operator bool() const { return good(); }

	// is good?
	bool good() const { return _state <= _eof; }

	// is eof?
	bool eof() const { return _state == _eof; }

	// is fail?
	bool fail() const { return _state == _fail; }

	//---other
	void reset() { _cur = 0; _state = _good; }

	//---cur helper
	// offset n bytes
	void offset(int n) { n >= 0 ? inc((size_t)n) : dec((size_t)-n); }

protected:
	// clear data but not delete buffer, care for memory leak!
	inline void clear_ex() { _data = 0; _cur = 0; _cap = 0; _state = _good; }
	inline void state(int state_) {
		_state = state_;
		/*if (_state & _fail)
		{
		WarningLn("buffer.state = fail");
		debugBreak;
		}*/
	}
	// check eof
	inline void check_eof() { if (_cur == 0 || _cur == _cap) state(_eof); }
	inline size_t left() const { return _cur; }
	inline size_t right() const { return _cap - _cur; }

	// increase n bytes
	inline void inc(size_t n) { if (n <= right()) _cur += n; else state(_fail); check_eof(); }

	// decrease n bytes
	inline void dec(size_t n) { if (n <= left()) _cur -= n; else state(_fail); check_eof(); }

	// safe copy but not case for state
	inline void copy(const void* buf, size_t bytes)
	{
		if (_data && buf && bytes <= right())
		{
			if (bytes > 0)
				memcpy(current(), buf, bytes);
			inc(bytes);
			check_eof();
		}
		else
		{
			state(_fail);
		}
	}
};


// 空分配器(适用于外部传入的固定buffer)
class buffer_allocator_dummy
{
public:
	size_t init_size() const { return 0; }
	void* _alloc(size_t bytes, size_t& capacity)
	{
		bytes; capacity;
		return nullptr;
	}
	void* _realloc(void* ptr, size_t old_size, size_t new_size, size_t& capacity)
	{
		ptr; old_size; new_size; capacity;
		return nullptr;
	}
	void _free(void* ptr)
	{
		ptr;
	}
};


// 普通的堆分配器
template<size_t _init_size = 256>
class buffer_allocator_heap
{
public:
	size_t init_size() const { return _init_size; }
	void* _alloc(size_t bytes, size_t& capacity)
	{
		if (bytes == 0) bytes = _init_size;
		unsigned char* buf = new unsigned char[bytes];
		if (!buf) bytes = 0;
		capacity = bytes;
		return buf;
	}
	void* _realloc(void* ptr, size_t old_size, size_t new_size, size_t& capacity)
	{
		assert((ptr && old_size) || (!ptr && old_size == 0) && new_size > 0);
		if (!ptr || new_size > capacity)
		{
			void* buf = this->_alloc(new_size, capacity);
			if (buf && ptr && old_size > 0)
				memcpy(buf, ptr, old_size);
			if (ptr) this->_free(ptr);
			return buf;
		}
		return ptr;
	}
	void _free(void* ptr)
	{
		delete[] ptr;
	}
};



// 栈分配器
template <size_t _init_size = 256>
class buffer_allocator_stack
{
protected:
	enum { _buf_size = _init_size ? ((_init_size + 7) & ~7) : 8 }; // 内置8字节对齐
	unsigned char _buf[_buf_size];
public:
	size_t init_size() const { return _init_size; }
	void* _alloc(size_t bytes, size_t& capacity)
	{
		if (bytes > _buf_size) bytes = 0;
		capacity = bytes;
		return bytes ? _buf : 0;
	}
	void* _realloc(void* ptr, size_t old_size, size_t new_size, size_t& capacity)
	{
		return this->_alloc(new_size, capacity);
	}
	void _free(void* ptr) { }
};



// 组合了栈和堆的分配器，优先使用栈，空间不够时才改用堆
template <size_t _init_size = 256>
class buffer_allocator_stack_or_heap :
	public buffer_allocator_stack<_init_size>,
	public buffer_allocator_heap<_init_size>
{
	typedef buffer_allocator_heap<_init_size>	heap;
	typedef buffer_allocator_stack<_init_size>	stack;
public:
	size_t init_size() const { return _init_size; }
	void* _alloc(size_t bytes, size_t& capacity)
	{
		if (bytes <= stack::_buf_size)
			return stack::_alloc(bytes, capacity);
		return heap::_alloc(bytes, capacity);

	}
	void* _realloc(void* ptr, size_t old_size, size_t new_size, size_t& capacity)
	{
		if (new_size <= stack::_buf_size)
		{
			if (old_size <= stack::_buf_size)
				return stack::_realloc(ptr, old_size, new_size, capacity);
			else
			{
				void* buf = stack::_alloc(new_size, capacity);
				if (buf && ptr && old_size > 0)
					memcpy(buf, ptr, new_size);
				this->_free(ptr);
				return buf;
			}
		}
		else if (ptr == stack::_buf)
		{
			void* buf = heap::_alloc(new_size, capacity);
			if (buf && old_size > 0)
				memcpy(buf, stack::_buf, old_size);
			return buf;
		}
		else
			return heap::_realloc(ptr, old_size, new_size, capacity);
	}
	void _free(void* ptr)
	{
		if (ptr != stack::_buf)
			heap::_free(ptr);
	}
};



// 在分配的尺寸大小作了优化的分配器，适配其他分配器
template<class _allocator>
class buffer_allocator_ex : public _allocator
{
	typedef _allocator base;
public:
	void* _alloc(size_t bytes, size_t& capacity)
	{
		size_t align_size = bytes ? ((bytes + 7) & ~7) : 8; // 内置8字节对齐
		return base::_alloc(align_size, capacity);
	}
	void* _realloc(void* ptr, size_t old_size, size_t new_size, size_t& capacity)
	{
		assert((ptr && old_size) || (!ptr && old_size == 0) && new_size > 0);
		if (!ptr || new_size > capacity)
		{
			size_t good_size = capacity ? capacity : new_size;
			while (good_size < new_size)
				good_size += (good_size >> 1); // 节约式递增
			size_t align_size = good_size ? ((good_size + 7) & ~7) : 8; // 内置8字节对齐

			return base::_realloc(ptr, old_size, good_size, capacity);
		}
		return ptr;
	}
};




/**
out buffer template
<buf_strategy> is a strategy above, or user custom strategy
*/
template <class buffer_allocator>
class obuffer : public basic_buffer, public buffer_allocator
{
	typedef basic_buffer				base;
	typedef buffer_allocator			alct;
	typedef obuffer<buffer_allocator>	mytype;
public:
	obuffer()
	{
		base::_data = (unsigned char*)alct::_alloc(alct::init_size(), base::_cap);
	}
	obuffer(unsigned char* buf, size_t maxsize) : base(buf, maxsize)
	{
	}
	obuffer(const mytype& rhs) // copy construction 
	{
		assign(rhs);
	}
	~obuffer()
	{
		clear();
	}
	void clear()
	{
		if (base::data())
			alct::_free(base::data());
		base::clear_ex();
	}
	mytype& operator=(const mytype& rhs)
	{
		return (assign(rhs));
	}

	mytype& assign(const mytype& rhs)
	{
		if (!base::data())
			base::_data = (unsigned char*)alct::_alloc(alct::init_size(), base::_cap);

		base::reset();
		if (rhs.size() > 0)
		{
			push_back(rhs.data(), rhs.size());
		}
		return (*this);
	}

	obuffer& push_back(const void* buf, size_t bytes)
	{
		// check args
		if (buf == 0 || bytes == 0)
		{
			base::state(base::_fail);
			return (*this);
		}

		// resize
		if (bytes > base::right())
		{
			unsigned char* ret = (unsigned char*)alct::_realloc(base::_data, base::_cur, base::_cur + bytes, base::_cap);
			if (!ret)
			{
				base::state(base::_fail);
				return (*this);
			}
			base::set_data(ret);
		}

		// copy
		base::copy(buf, bytes);

		return (*this);
	}

	void* span(size_t bytes)
	{
		// check args
		if (bytes == 0)
		{
			base::state(base::_fail);
			return nullptr;
		}

		// resize
		if (bytes > base::right())
		{
			unsigned char* ret = (unsigned char*)alct::_realloc(base::_data, base::_cur, base::_cur + bytes, base::_cap);
			if (!ret)
			{
				base::state(base::_fail);
				return nullptr;
			}
			base::set_data(ret);
		}

		void* p = current();
		inc(bytes);

		return p;
	}

	/// 该方法仅仅支持对象的浅拷贝，如果对象需要进行深拷贝，需要定制该操作符
	template <class T> obuffer& operator << (T value)
	{
		// resize
		if (sizeof(T) > base::right())
		{
			unsigned char* ret = (unsigned char*)alct::_realloc(base::_data, base::_cur, base::_cur + sizeof(T), base::_cap);
			if (!ret)
			{
				base::state(base::_fail);
				return (*this);
			}
			base::set_data(ret);
		}

		// assign
		if (base::current() && sizeof(T) <= right())
		{
			*(T*)base::current() = value;
			base::inc(sizeof(T));
			base::check_eof();
		}
		else
		{
			base::state(base::_fail);
		}
		return (*this);
	}

	/// 对常字符串的特化处理
	obuffer& operator << (const char* value)
	{
		if (value)
			return push_back((const void*)value, strlen(value) + sizeof(char));;
		base::state(base::_fail);
		return *this;
	}

	// 对字符串的特化处理
	obuffer& operator << (char* value)
	{
		return operator << ((const char*)value);
	}

	// 对常宽字符串的特化处理
	obuffer& operator << (const wchar_t* value)
	{
		if (value)
			return push_back((const void*)value, (wcslen(value) + 1) * sizeof(wchar_t));
		base::state(base::_fail);
		return *this;
	}

	// 对宽字符串的特化处理
	obuffer& operator << (wchar_t* value)
	{
		return operator << ((const wchar_t*)value);
	}

	// 对字符串类的处理
	obuffer& operator << (const std::string& value)
	{
		return operator << (value.c_str());
	}

	obuffer& operator << (std::string& value)
	{
		return operator << (value.c_str());
	}

	obuffer& operator << (const std::wstring& value)
	{
		return operator << (value.c_str());
	}

	obuffer& operator << (std::wstring& value)
	{
		return operator << (value.c_str());
	}

	// 跳过某种数据类型（不进行赋值，仅仅改变当前位置）
	template <class T> obuffer& skip()
	{
		base::inc(sizeof(T));
		return (*this);
	}
};




// 输入缓冲（指从缓冲区中导出各种数据）
class ibuffer : public basic_buffer
{
	typedef basic_buffer		base;
	// 构造（析构）
protected:
	ibuffer() : basic_buffer() { }

public:
	ibuffer(const void* buffer, size_t actualSize) : basic_buffer((unsigned char*)buffer, actualSize) { }

	~ibuffer() { }


	// 方法
public:
	// 获取缓冲区中数据的实际占用尺寸
	size_t size() const
	{
		return cap();
	}

	// 从缓冲区导出各种具有固定长度的数据类型，包括简单类型和复合类型（结构）
	// 该方法仅仅支持对象的浅拷贝，如果对象需要进行深拷贝，需要定制该操作符
	template <class T> ibuffer& operator >> (T& value)
	{
		if (sizeof(T) <= base::right())
		{
			value = *(T*)base::current();
			base::inc(sizeof(T));
		}
		else
		{
			base::state(base::_fail);
		}
		return (*this);
	}

	// 对常字符串的特化处理
	ibuffer& operator >> (const char*& value)
	{
		unsigned char* str = base::current();
		while (str < base::end() && *str++);
		size_t bytes = (size_t)(str - base::current());
		if (bytes > 0 && bytes <= base::right())
		{
			if (*(base::current() + bytes - sizeof(char)) != 0) // 不是0结尾的字符串?
			{
				base::state(base::_eof | base::_fail);
				return (*this);
			}
			value = (char*)base::current();
			base::inc(bytes);
		}
		else
		{
			value = 0;
			base::state(base::_eof | base::_fail);
		}

		return (*this);
	}

	// 对字符串的特化处理
	ibuffer& operator >> (char*& value)
	{
		return operator>>((const char*&)value);
	}

	// 对常宽字符串的特化处理
	ibuffer& operator >> (const wchar_t*& value)
	{
		wchar_t* str = (wchar_t*)base::current();
		wchar_t* end_ = (wchar_t*)base::end();
		while (str < end_ && *str++);
		size_t bytes = (size_t)((unsigned char*)str - base::current());
		if (bytes > 0 && bytes <= base::right())
		{
			if (*(const wchar_t*)(base::current() + bytes - sizeof(wchar_t)) != 0) // 不是0结尾?
			{
				base::state(base::_eof | base::_fail);
				return (*this);
			}
			value = (wchar_t*)base::current();
			base::inc(bytes);
		}
		else
		{
			value = 0;
			base::state(base::_eof | base::_fail);
		}

		return (*this);
	}

	/// 对宽字符串的特化处理
	ibuffer& operator >> (wchar_t*& value)
	{
		return operator>>((const wchar_t*&)value);
	}

	// 对字符串类的特化处理
	ibuffer& operator >> (std::string& value)
	{
		value.clear();
		const char* str = 0;
		operator>>((const char*&)str);
		if (str)
			value = str;

		return (*this);
	}

	ibuffer& operator >> (std::wstring& value)
	{
		value.clear();
		const wchar_t* str = 0;
		operator>>((const wchar_t*&)str);
		if (str)
			value = str;

		return (*this);
	}

	// 跳过某种数据类型（不进行赋值，仅仅改变当前位置）
	template <class T> ibuffer& skip()
	{
		base::inc(sizeof(T));
		return (*this);
	}

	// 在当前位置导出一段缓冲
	ibuffer& pop(void* buffer, size_t bytes)
	{
		if (buffer == 0 || bytes == 0)
		{
			base::state(base::_fail);
			return (*this);
		}

		if (bytes <= base::right())
		{
			memcpy(buffer, base::current(), bytes);
			base::inc((bytes));
		}
		else
		{
			base::state(base::_eof | base::_fail);
		}

		return (*this);
	}
};




// 栈buffer
#ifndef STACKOBUFFER_TYPEDEF
#	define STACKOBUFFER_TYPEDEF
	/*
	typedef obuffer<buffer_allocator_stack<32>    >		obuf32;		/// 类似于 unsigned char buf[32];
	typedef obuffer<buffer_allocator_stack<64>    >		obuf64;		/// 类似于 unsigned char buf[64];
	typedef obuffer<buffer_allocator_stack<128>   >		obuf128;	/// 类似于 unsigned char buf[128];
	typedef obuffer<buffer_allocator_stack<256>   >		obuf256;	/// 类似于 unsigned char buf[256];
	typedef obuffer<buffer_allocator_stack<512>	  >		obuf512;	/// 类似于 unsigned char buf[512];
	typedef obuffer<buffer_allocator_stack<1024>  >		obuf1024;	/// 类似于 unsigned char buf[1024];
	typedef obuffer<buffer_allocator_stack<2048>  >		obuf2048;	/// 类似于 unsigned char buf[2048];
	typedef obuffer<buffer_allocator_stack<4096>  >		obuf4096;	/// 类似于 unsigned char buf[4096];
	*/

// 以栈优先，空间不够时自动切换为堆来保存数据
typedef obuffer<buffer_allocator_ex<buffer_allocator_stack_or_heap<32> > >		obuf32;
typedef obuffer<buffer_allocator_ex<buffer_allocator_stack_or_heap<64> > >		obuf64;
typedef obuffer<buffer_allocator_ex<buffer_allocator_stack_or_heap<128> > >		obuf128;
typedef obuffer<buffer_allocator_ex<buffer_allocator_stack_or_heap<256> > >		obuf256, obuf;
typedef obuffer<buffer_allocator_ex<buffer_allocator_stack_or_heap<512> > >		obuf512;
typedef obuffer<buffer_allocator_ex<buffer_allocator_stack_or_heap<1024> > >	obuf1024;
typedef obuffer<buffer_allocator_ex<buffer_allocator_stack_or_heap<2048> > >	obuf2048;
typedef obuffer<buffer_allocator_ex<buffer_allocator_stack_or_heap<4096> > >	obuf4096;
typedef obuffer<buffer_allocator_ex<buffer_allocator_stack_or_heap<8192> > >	obuf8192;
typedef obuffer<buffer_allocator_ex<buffer_allocator_stack_or_heap<10240> > >	obuf10240;
typedef obuffer<buffer_allocator_ex<buffer_allocator_stack_or_heap<16384> > >	obuf16384;

typedef obuffer<buffer_allocator_dummy>				ofixbuf;	// 使用外部缓冲区

#endif

#endif // _BUFFER_H
