#ifndef __AIT_SOCKET_H__
#define __AIT_SOCKET_H__
/*
This header must inlude before "windows.h"
such as:
#include "ait_socket.h"
#include <Windows.h>

author: cuimolei
e-mail: cuimolei@163.com
*/
#pragma once
#pragma comment(lib,"ws2_32.lib") 

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <WinSock2.h>


#ifdef AIT_RET_NONE
#undef AIT_RET_NONE
#endif // !SOCKET_RET_NONE
#define AIT_RET_NONE

#ifndef AIT_CEHCK_ERR
#define AIT_CEHCK_ERR(condition, ERR_ID, ret_val) \
do{ \
if (condition) \
{ \
fprintf_s(stderr, "%s %d error: %x\n", __FILE__, __LINE__, ERR_ID); \
return  ret_val; \
}\
} while (0)
#endif // !AIT_CEHCK_ERR

#define SOCKET_CHECK_ERR(ret, ERR_ID) \
	AIT_CEHCK_ERR(ret == ERR_ID, WSAGetLastError(), ERR_ID)

class AitSocketServer
{
	class AitWSA
	{
	public:
		AitWSA()
		{
			WSADATA wsa_data;
			int ret = 0;
			ret = WSAStartup(MAKEWORD(2, 2), &wsa_data);
		}
		~AitWSA()
		{
			WSACleanup();
		}
	};

	SOCKET listen_socket;

public:
	AitSocketServer()
	{
		volatile static AitWSA ait_wsa_hdl;
		this->listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}

	~AitSocketServer()
	{
		this->free();
	}

	int free()
	{
		int ret = closesocket(listen_socket);
		SOCKET_CHECK_ERR(ret, SOCKET_ERROR);
		listen_socket = INVALID_SOCKET;
		return ret;
	}

	int init(uint16_t port, const char * ip = "127.0.0.1", int max_connect = 1)
	{
		int ret = 0;
		SOCKADDR_IN addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(ip);
		addr.sin_port = htons(25001);
		ret = bind(this->listen_socket, (SOCKADDR *)&addr, sizeof(SOCKADDR_IN));
		SOCKET_CHECK_ERR(ret, SOCKET_ERROR);

		//Listen for incoming request
		ret = listen(listen_socket, max_connect);
		SOCKET_CHECK_ERR(ret, SOCKET_ERROR);

		return ret;
	}

	SOCKET getListener()
	{

		SOCKET_CHECK_ERR(this->listen_socket, INVALID_SOCKET);
		return this->listen_socket;
	}

	SOCKET getAcceptor(struct sockaddr_in *p_addr_in = NULL)
	{
		struct sockaddr_in cl_addr;
		int cl_addr_len = sizeof(cl_addr);
		SOCKET accept_socket = accept(listen_socket, (struct sockaddr *)&cl_addr, &cl_addr_len);
		SOCKET_CHECK_ERR(accept_socket, INVALID_SOCKET);

		printf_s("Client connected.\n");
		this->dumpAddrIn(cl_addr);

		if (p_addr_in)
		{
			*p_addr_in = cl_addr;
		}
		return accept_socket;
	}

	void dumpAddrIn(struct sockaddr_in &addr_in)
	{
		printf_s("Client IP: %s\n", inet_ntoa(addr_in.sin_addr));
		printf_s("Client Port: %hu\n", ntohs(addr_in.sin_port));
	}

};

class AitSocketIO
{
	std::string buf;
	size_t max_len;
	SOCKET socket;
public:
	AitSocketIO(SOCKET socket, size_t init_len = 1500)
	{
		this->socket = socket;
		this->max_len = init_len;
		this->buf.resize(init_len);
	}

	int reciveMessage(std::string &message)
	{
		int ret = 0;
		int32_t buf_len;

		ret = recv(this->socket, (char *)&buf_len, sizeof(int32_t), 0);
		SOCKET_CHECK_ERR(ret, SOCKET_ERROR);

		ret = send(this->socket, (char *)&buf_len, sizeof(int32_t), 0);
		SOCKET_CHECK_ERR(ret, SOCKET_ERROR);

		message.resize(buf_len);
		ret = recv(this->socket, (char *)message.data(), message.size(), 0);
		SOCKET_CHECK_ERR(ret, SOCKET_ERROR);

		return 0;
	}

	int sendMessage(std::string &message)
	{
		int ret = 0;
		int32_t buf_len = message.size();

		ret = send(this->socket, (char *)&buf_len, sizeof(int32_t), 0);
		SOCKET_CHECK_ERR(ret, SOCKET_ERROR);
		AIT_CEHCK_ERR(buf_len < 0, SOCKET_ERROR, SOCKET_ERROR);

		ret = recv(this->socket, (char *)&buf_len, sizeof(int32_t), 0);
		SOCKET_CHECK_ERR(ret, SOCKET_ERROR);

		message.resize(buf_len);
		ret = send(this->socket, (char *)message.data(), message.size(), 0);
		SOCKET_CHECK_ERR(ret, SOCKET_ERROR);
		return 0;
	}

	template<typename T> int sendObject(T &obj)
	{
		size_t len = sizeof(T);
		if (len > this->max_len)
		{
			this->max_len = len;
			this->buf.resize(this->max_len);
		}
		memcpy((void *)this->buf.data(), &obj, len);
		return sendMessage(this->buf);
	}

	template<typename T> int reciveObject(T &obj)
	{
		int ret = reciveMessage(this->buf);
		AIT_CEHCK_ERR(ret < 0, SOCKET_ERROR, SOCKET_ERROR);
		if (this->max_len < this->buf.size())
		{
			this->max_len = this->buf.size();
		}
		memcpy(&obj, this->buf.data(), sizeof(T));
		return 0;
	}
};


class AitSocketClient
{
#ifndef SOCKET_CHECK_ERR
#define SOCKET_CHECK_ERR(ret, ERR_ID) \
do{ \
	if (ret == ERR_ID) \
	{ \
		fprintf_s(stderr, "%s %d error: %ld\n", __FILE__, __LINE__, WSAGetLastError()); \
		return  ERR_ID; \
	}\
} while (0)
#endif // !SOCKET_CHECK_ERR
	class AitWSA
	{
	public:
		AitWSA()
		{
			WSADATA wsa_data;
			int ret = 0;
			ret = WSAStartup(MAKEWORD(2, 2), &wsa_data);
		}
		~AitWSA()
		{
			WSACleanup();
		}
	};

public:
	AitSocketClient()
	{
		volatile static AitWSA ait_wsa_hdl;
	}

	SOCKET getConnector(uint16_t port, const char * ip = "127.0.0.1")
	{
		int ret;
		//Create socket
		SOCKET connect_socet;
		connect_socet = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		SOCKET_CHECK_ERR(connect_socet, INVALID_SOCKET);

		//Set sever addr and port
		SOCKADDR_IN addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(ip);
		addr.sin_port = htons(25001);
		ret = connect(connect_socet, (SOCKADDR *)&addr, sizeof(SOCKADDR_IN));
		AIT_CEHCK_ERR(ret == SOCKET_ERROR, WSAGetLastError(), INVALID_SOCKET);

		return connect_socet;
	}
};

#endif // !__AIT_SOCKET_H__

