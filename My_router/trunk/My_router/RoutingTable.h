#pragma once
#include "stdafx.h"
#include "Utils.h"

typedef struct
{
	in_addr ip_address;
	unsigned long mask;
} Address;

typedef struct
{
	char router_name[MAX_ROUTER_NAME];
	unsigned long cost; //Cost to subnet through router in name
} RouterAddress;

typedef struct
{
	in_addr router_ip;		
	unsigned short port;
	unsigned long cost_to_router;
	Subnet via_subnet;
	bool alive;
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

	//************************************
	// Method:    GetRouterSubnet
	// FullName:  RoutingTable::GetRouterSubnet
	// Access:    public 
	// Returns:   Utils::ReturnStatus
	// Qualifier: Gets the best connecting subnet to a neighbor
	// Parameter: __in RouterEntry * router
	// Parameter: __out Subnet * subnet
	//************************************
	static Utils::ReturnStatus GetRouterSubnet(__in RouterEntry* router, __out Subnet* subnet);

	//************************************
	// Method:    ModifyRoute
	// FullName:  RoutingTable::ModifyRoute
	// Access:    public 
	// Returns:   Utils::ReturnStatus
	// Qualifier: Modifies the cost to a subnet via a neighbor
	// Parameter: __in char name[MAX_ROUTER_NAME] - Name of the router to modify
	// Parameter: __in Subnet * subnet_ptr - The subnet that the cost from this router has changed
	//************************************
	static Utils::ReturnStatus ModifyRoute(__in char name[MAX_ROUTER_NAME], __in Subnet* subnet_ptr);

	//************************************
	// Method:    AddRouter
	// FullName:  RoutingTable::AddRouter
	// Access:    public 
	// Returns:   Utils::ReturnStatus
	// Qualifier: Adds router to router's map for fast data extraction
	// Parameter: __in char name[MAX_ROUTER_NAME]
	// Parameter: __in in_addr actual_router_ip
	// Parameter: __in unsigned short port
	// Parameter: __in Subnet * subnet_ptr
	// Parameter: __in unsigned short cost
	//************************************
	static Utils::ReturnStatus AddRouter(__in char name[MAX_ROUTER_NAME],__in in_addr actual_router_ip, 
									 	 __in unsigned short port, __in Subnet* subnet_ptr, __in unsigned long cost);

	//************************************
	// Method:    ReportDeadRouter
	// FullName:  RoutingTable::ReportDeadRouter
	// Access:    public static 
	// Returns:   Utils::ReturnStatus
	// Qualifier: Handles dead routers
	// Parameter: __in char name[MAX_ROUTER_NAME]
	//************************************
	static Utils::ReturnStatus ReportDeadRouter(__in char name[MAX_ROUTER_NAME]);

	//************************************
	// Method:    PrintMap
	// FullName:  RoutingTable::PrintMap
	// Access:    public 
	// Returns:   void
	// Qualifier: Prints to screen the router's map
	//************************************
	static void PrintMap();

protected:
	//************************************
	// Method:    CompareSubnets
	// FullName:  RoutingTable::CompareSubnets
	// Access:    protected static 
	// Returns:   bool
	// Qualifier: Compares two subnets
	// Parameter: Address first_address
	// Parameter: Address second_address
	//************************************
	static bool CompareSubnets(Address first_address, Address second_address);

	//Sorted vector for each router. First element is the best route
	static vector<RoutingTableEntry> *m_routing_table;

	//Router's details
	static RoutersMap* m_routers_map;
};
