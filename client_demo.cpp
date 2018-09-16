// demo_32bit.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include "ait_socket.h"
#include <Windows.h>

typedef struct
{
	int a;
	float b;
	char c[3];
} TestStruct;

int main()
{
	
	AitSocketClient client;

	SOCKET client_socket = client.getConnector(25001);

	AitSocketIO socket_io(client_socket);

	TestStruct test_struct;
	test_struct.a = 1;
	test_struct.b = 2;
	strcpy(test_struct.c, "3");

	socket_io.sendObject(test_struct);


    return 0;
}

