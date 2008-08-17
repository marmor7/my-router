#pragma once
#include "stdafx.h"
#include "Utils.h"

typedef int Router; //TBD: Change to actual return type.

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
	// Parameter: char name[MAX_ROUTER_NAME]
	// Parameter: Subnet * subnet_ptr
	//************************************
	static Utils::ReturnStatus AddRoute(char name[MAX_ROUTER_NAME], Subnet* subnet_ptr);

	//************************************
	// Method:    GetBestRoute
	// FullName:  RoutingTable::GetBestRoute
	// Access:    public static 
	// Returns:   Router
	// Qualifier: Gets best route for address
	// Parameter: in_addr address
	//************************************
	static Router GetBestRoute(in_addr address);

private:
	//Sorted list for each router. First element is the best route
	static map<const string, vector<Subnet*> > m_routing_table;	
};
