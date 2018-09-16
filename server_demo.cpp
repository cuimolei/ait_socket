// server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include "ait_socket.h"


typedef struct
{
	int a;
	float b;
	char c[3];
} TestStruct;

int main()
{
	
	int ret = 0;
	//Init socket

	AitSocketServer server;

	server.init(25001, "127.0.0.1", 1);

	SOCKET accept_socket = server.getAcceptor();

	AitSocketIO socket_io(accept_socket);


	TestStruct test_struct;

	socket_io.reciveObject(test_struct);

	printf_s("a = %d, b = %f, c = %s\r\n", test_struct.a, test_struct.b, test_struct.c);

	closesocket(accept_socket);

	return 0;
	
}



