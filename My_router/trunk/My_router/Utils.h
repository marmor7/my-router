#pragma once
#include "stdafx.h"

using namespace std;

#define DEBUG_NONE 0
#define DEBUG_IMPORTANT 1
#define DEBUG_TRACE 2
#define DEBUG_ALL 3

#define DEBUG_LEVEL DEBUG_TRACE

#define IF_DEBUG(level) if (DEBUG_##level <= DEBUG_LEVEL)

#define NUM_OF_ROUTERS 32
#define MAX_ROUTER_NAME 8

struct RouterEntry{
	int socketId;
	char name[MAX_ROUTER_NAME];
	in_addr address;
	short port;
	bool neighbour;
};

class Utils
{
public:
	Utils();
	~Utils();

	//Enumerator of the possible return statuses by InitRouter.
	enum ReturnStatus
	{
		STATUS_OK,
		STATUS_BAD_NUM_OF_ARGUMENTS,
		STATUS_BAD_FILENAME_OR_FILE_DONT_EXISTS,
		STATUS_BAD_IP_FORMAT,
	};

	enum SocketReturnStatus
	{
		STATUS_SOCKET_OK,
		STATUS_BAD_SOCKET,
		STATUS_BAD_CONNECT, 
		STATUS_BAD_ACCEPT,
		STATUS_SEND_OK,
		STATUS_SEND_FAILED
	};
};
