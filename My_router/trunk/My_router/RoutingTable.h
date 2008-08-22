#pragma once
#include "stdafx.h"
#include "Utils.h"

typedef struct
{
	in_addr ip_address;
	short int mask;
} Address;

typedef struct
{
	char* router_name[MAX_ROUTER_NAME];
	in_addr router_ip;
	short int cost;
} RouterAddress;

typedef pair<Address, list<RouterAddress>* > RoutingTableEntry;

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
	// Parameter: Subnet * subnet_ptr - Router's spanned IP
	//************************************
	static Utils::ReturnStatus AddRoute(__in char name[MAX_ROUTER_NAME],__in in_addr actual_router_ip, __in Subnet* subnet_ptr);

	//************************************
	// Method:    GetBestRoute
	// FullName:  RoutingTable::GetBestRoute
	// Access:    public static 
	// Returns:   in_addr - Actual IP address to route you message to (best next link)
	// Qualifier: Gets best route for address
	// Parameter: in_addr address
	//************************************
	static in_addr GetBestRoute(in_addr address);

protected:
	static bool CompareSubnets(Address first_address, Address second_address);

	//Sorted list for each router. First element is the best route
	static vector<RoutingTableEntry> *m_routing_table;
};
