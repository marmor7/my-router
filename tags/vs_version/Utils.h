#pragma once
#include "stdafx.h"
#include "MyRIPMessage.h"

using namespace std;

#define DEBUG_ERROR 0
#define DEBUG_IMPORTANT 1
#define DEBUG_TRACE 2
#define DEBUG_ALL 3

#define DEBUG_LEVEL DEBUG_ERROR

#define IF_DEBUG(level) if (DEBUG_##level <= DEBUG_LEVEL)

#define NUM_OF_ROUTERS 32
#define MAX_ROUTER_NAME 8
#define MAX_SUBNETS_FOR_SINGLE_ROUTER 8
#define SIZE_OF_RIP_MSG 412

#define INFINITY 255 //Defined by course staff

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
	int cost;
	int mask;
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
	//************************************
	// Method:    ~Utils
	// FullName:  Utils::~Utils
	// Access:    public 
	// Returns:   
	// Qualifier: Destructor
	//************************************
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

	//Enum of the possible return statuses by sockets usage
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

	//************************************
	// Method:    PrintMsg
	// FullName:  Utils::PrintMsg
	// Access:    public 
	// Returns:   void
	// Qualifier: Prints a message content
	// Parameter: MyRIPMessage * msg
	//************************************
	static void PrintMsg(MyRIPMessage* msg);

	//************************************
	// Method:    PrintDest
	// FullName:  Utils::PrintDest
	// Access:    public 
	// Returns:   Utils::ReturnStatus
	// Qualifier: Prints destination property
	// Parameter: int i
	// Parameter: DestinationProperties * dest
	//************************************
	static Utils::ReturnStatus PrintDest(int i, DestinationProperties* dest);

	//************************************
	// Method:    Host2netMsg
	// FullName:  Utils::Host2netMsg
	// Access:    public 
	// Returns:   Utils::ReturnStatus
	// Qualifier: Converts a message from host order to net order
	// Parameter: MyRIPMessage * msg
	//************************************
	static Utils::ReturnStatus Host2netMsg(MyRIPMessage* msg);

	//************************************
	// Method:    Net2hostMsg
	// FullName:  Utils::Net2hostMsg
	// Access:    public 
	// Returns:   Utils::ReturnStatus
	// Qualifier: Converts message from net order to host order
	// Parameter: MyRIPMessage * msg
	//************************************
	static Utils::ReturnStatus Net2hostMsg(MyRIPMessage* msg);

private:
	//************************************
	// Method:    Utils
	// FullName:  Utils::Utils
	// Access:    private 
	// Returns:   
	// Qualifier: Private constructor
	//************************************
	Utils();
};
