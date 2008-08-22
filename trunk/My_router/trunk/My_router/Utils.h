#pragma once
#include "stdafx.h"
#include "MyRIPMessage.h"

using namespace std;

#define DEBUG_ERROR 0
#define DEBUG_IMPORTANT 1
#define DEBUG_TRACE 2
#define DEBUG_ALL 3

#define DEBUG_LEVEL DEBUG_TRACE

#define IF_DEBUG(level) if (DEBUG_##level <= DEBUG_LEVEL)

#define NUM_OF_ROUTERS 32
#define MAX_ROUTER_NAME 8
#define MAX_SUBNETS_FOR_SINGLE_ROUTER 8
#define SIZE_OF_RIP_MSG 156
#define TIMEOUT_SEC_MIN 20
#define TIMEOUT_SEC_MAX 40

#define ROUTER_PORT	44444

#define INFINITY (SHRT_MAX-1)

struct RouterEntry
{
	//int socketId;
	char name[MAX_ROUTER_NAME];
	in_addr address;
	short port;
	bool neighbour;
	//byte msg[SIZE_OF_RIP_MSG];
	//int msg_len;
};

struct Subnet
{
	in_addr address;
	short int cost;
	short int mask;
};

//Functor to compare 2 subnets by STL sort
struct CompareSubnetsByCost
{
	bool operator()(Subnet* subnet1, Subnet* subnet2)
	{
		return subnet1->cost < subnet2->cost;
	}
};

class Utils
{
public:
	Utils();
	~Utils();

	static void PrintMsg(MyRIPMessage* msg);
	static void PrintDest(int i, DestinationProperties* dest);

	//Enumerator of the possible return statuses by InitRouter.
	enum ReturnStatus
	{
		STATUS_OK,
		STATUS_BAD_NUM_OF_ARGUMENTS,
		STATUS_BAD_FILENAME_OR_FILE_DONT_EXISTS,
		STATUS_BAD_IP_FORMAT,
		STATUS_BAD_PARAMETER
	};

	enum SocketReturnStatus
	{
		STATUS_SOCKET_OK,
		STATUS_BAD_SOCKET,
		STATUS_BAD_CONNECT, 
		STATUS_BAD_ACCEPT,
		STATUS_SEND_OK,
		STATUS_SEND_FAILED,
		STATUS_RECIVE_OK,
		STATUS_RECIVE_FAILED,
		STATUS_CLOSE_OK,
		STATUS_CLOSE_FAILED,
		STATUS_BAD_BIND
	};
};
