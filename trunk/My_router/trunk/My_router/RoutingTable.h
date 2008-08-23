#pragma once
#include "stdafx.h"
#include "Utils.h"

typedef struct
{
	in_addr ip_address;
	unsigned short mask;
} Address;

typedef struct
{
	char router_name[MAX_ROUTER_NAME];
	unsigned short cost;
} RouterAddress;

typedef struct
{
	in_addr router_ip;
	unsigned short port;
	unsigned short cost_to_router;
	Subnet via_subnet;
} RouterDetails;

typedef pair<Address, vector<RouterAddress>* > RoutingTableEntry;
typedef map<string, RouterDetails> RoutersMap;

class RoutingTable
{
public:
	//************************************
	// Method:    RoutingTable
	// FullName:  RoutingTable::RoutingTable
	// Access:    public 
	// Returns:   
	// Qualifier: Constructor
	//************************************
	RoutingTable();

	//************************************
	// Method:    ~RoutingTable
	// FullName:  RoutingTable::~RoutingTable
	// Access:    public 
	// Returns:   
	// Qualifier: Destructor
	//************************************
	~RoutingTable();

	//************************************
	// Method:    PrintDV
	// FullName:  RoutingTable::PrintDV
	// Access:    public 
	// Returns:   void
	// Qualifier: Prints the DV Routing Table to screen
	//************************************
	void PrintDV();

	//************************************
	// Method:    GetDV
	// FullName:  RoutingTable::GetDV
	// Access:    public 
	// Returns:   void
	// Qualifier: Fill msg with DV data before message sending
	// Parameter: MyRIPMessage * msg
	//************************************
	void GetDV(MyRIPMessage* msg);

	//************************************
	// Method:    AddRoute
	// FullName:  RoutingTable::AddRoute
	// Access:    public 
	// Returns:   Utils::ReturnStatus
	// Qualifier: Adds route to DV table
	// Parameter: char name[MAX_ROUTER_NAME] - Router's name
	// Parameter: in_addr actual_router_ip - Actual router's IP
	// Parameter: unsigned short port - The router's port
	// Parameter: Subnet * subnet_ptr - Router's spanned IP
	//************************************
	static Utils::ReturnStatus AddRoute(__in char name[MAX_ROUTER_NAME],__in in_addr actual_router_ip, 
										__in unsigned short port, __in Subnet* subnet_ptr);

	//************************************
	// Method:    GetBestRoute
	// FullName:  RoutingTable::GetBestRoute
	// Access:    public static 
	// Returns:   RouterAddress - Actual IP address and port to route you message to (best next link)
	// Qualifier: Gets best route for address
	// Parameter: in_addr address
	//************************************
	static Utils::ReturnStatus GetBestRoute( in_addr address, RouterAddress* ra );

	//Gets the best connecting subnet to a neighbor
	static Utils::ReturnStatus GetRouterSubnet(__in RouterEntry* router, __out Subnet* subnet);

	//Modifies the cost to a subnet via a neighbor
	static Utils::ReturnStatus ModifyRoute(__in char name[MAX_ROUTER_NAME],__in in_addr actual_router_ip, 
		__in unsigned short port, __in Subnet* subnet_ptr) {return Utils::STATUS_OK;};

	static Utils::ReturnStatus AddRouter(__in char name[MAX_ROUTER_NAME],__in in_addr actual_router_ip, 
		__in unsigned short port, __in Subnet* subnet_ptr, __in unsigned short cost);

	static void PrintMap();

protected:
	static bool CompareSubnets(Address first_address, Address second_address);

	//Sorted list for each router. First element is the best route
	static vector<RoutingTableEntry> *m_routing_table;
	static RoutersMap* m_routers_map;
};
