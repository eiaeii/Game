#ifndef _GATEWAYSERVER_H
#define _GATEWAYSERVER_H

#include "Singleton.hpp"
#include "IThread.h"
#include "Log.h"
#include "zmq.h"

class CGatewayServer : public IThread,
	public CSingleton<CGatewayServer>
{
public:
	~CGatewayServer();
	bool InitServer();
	bool Start();

	void s_dump()
	{
		int rc = 0;

		zmq_msg_t message;
		rc = zmq_msg_init(&message);
		
		//  Process all parts of the message
		do {
			int size = zmq_msg_recv(&message, m_pZmqSocket, 0);
			puts("----------------------------------------");
			//  Dump the message as text or binary
			char *data = (char*)zmq_msg_data(&message);

			int is_text = 1;
			int char_nbr;
			for (char_nbr = 0; char_nbr < size; char_nbr++) {
				if ((unsigned char)data[char_nbr] < 32
					|| (unsigned char)data[char_nbr] > 126) {
					is_text = 0;
				}
			}

			printf("[%03d] ", size);
			for (char_nbr = 0; char_nbr < size; char_nbr++) {
				if (is_text) {
					printf("%c", data[char_nbr]);
				}
				else {
					printf("%02X", (unsigned char)data[char_nbr]);
				}
			}
			printf("\n");
		} while (zmq_msg_more(&message));

		rc = zmq_msg_close(&message);
	}

private:
	void ProcessLogic();
	bool BeginStop();

private:

	void *m_pZmqContext = nullptr;
	void *m_pZmqSocket = nullptr;
	zmq_msg_t m_zmqMsg;
};

#endif // _GATEWAYSERVER_H