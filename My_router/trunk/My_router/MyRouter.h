#pragma once;
#include "stdafx.h"
#include "EventHandler.h"
#include "RoutingTable.h"

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

	//Add a router (name, ip, port)
	Utils::ReturnStatus addRouter(char name[8], in_addr* address, short port);

	//Main loop of the router
	void MyRouter::run();

protected:
	//Router's name
	string m_name;

	EventHandler* handler;
	RoutingTable* table;
	RouterEntry* routers;
	int numOfRouters;

private:
};