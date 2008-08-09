#pragma once;
#include "stdafx.h"
#include "Utils.h"

//Fwd declarations
class EventHandler;
class RoutingTable;

using namespace std;

class MyRouter
{
public:
	//************************************
	// Method:    MyRouter
	// FullName:  MyRouter::MyRouter
	// Access:    public 
	// Returns:   
	// Qualifier: Constructor
	//************************************
	MyRouter();

	//************************************
	// Method:    ~MyRouter
	// FullName:  MyRouter::~MyRouter
	// Access:    public 
	// Returns:   
	// Qualifier: Destructor
	//************************************
	~MyRouter();

	//************************************
	// Method:    MyRouter
	// FullName:  MyRouter::MyRouter
	// Access:    public 
	// Returns:   
	// Qualifier: Constructor
	// Parameter: string name - Router's name
	//************************************
	MyRouter(string name);

	//************************************
	// Method:    GetName
	// FullName:  MyRouter::GetName
	// Access:    public 
	// Returns:   std::string
	// Qualifier: Gets router's name
	//************************************
	string GetName();

	//************************************
	// Method:    SetName
	// FullName:  MyRouter::SetName
	// Access:    public 
	// Returns:   void
	// Qualifier: Sets a new name to the router
	// Parameter: string name
	//************************************
	void SetName(string name);

	//************************************
	// Method:    AddRouter
	// FullName:  MyRouter::AddRouter
	// Access:    public 
	// Returns:   Utils::ReturnStatus
	// Qualifier: Add a router (name, ip, port)
	// Parameter: char name[8]
	// Parameter: in_addr * address
	// Parameter: short port
	//************************************
	Utils::ReturnStatus AddRouter(char name[8], in_addr* address, short port);

	//************************************
	// Method:    Run
	// FullName:  MyRouter::Run
	// Access:    public 
	// Returns:   void
	// Qualifier: Main loop of the router
	//************************************
	void Run();

protected:
	//Router's name
	string m_name;

	//Incoming event handler
	EventHandler* m_handler;

	//Router's routing table
	RoutingTable* m_table;

	//???
	RouterEntry* m_routers;

	//Number of neighboring routers
	int m_num_of_routers;

private:
};