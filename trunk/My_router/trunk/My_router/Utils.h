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
#define SIZE_OF_RIP_MSG 412

#define INFINITY (SHRT_MAX-1)

struct buffer
{
	BYTE msg[SIZE_OF_RIP_MSG];//TBD: should we support more than one msg?
	int len;
};

struct RouterEntry
{
	char name[MAX_ROUTER_NAME];
	in_addr address;
	unsigned short port;
	bool reachable;
	timeval timeout;
	buffer out;
};

struct Subnet
{
	in_addr address;
	unsigned short int cost;
	unsigned short int mask;
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

	//Enumerator of the possible return statuses by InitRouter.
	enum ReturnStatus
	{
		STATUS_OK,
		STATUS_BAD_NUM_OF_ARGUMENTS,
		STATUS_BAD_FILENAME_OR_FILE_DONT_EXISTS,
		STATUS_BAD_IP_FORMAT,
		STATUS_BAD_PARAMETER,
		STATUS_STOP,
		STATUS_NOT_FOUND
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

	static void PrintMsg(MyRIPMessage* msg);
	static Utils::ReturnStatus PrintDest(int i, DestinationProperties* dest);

	static Utils::ReturnStatus host2netMsg(MyRIPMessage* msg);
	static Utils::ReturnStatus net2hostMsg(MyRIPMessage* msg);

};
