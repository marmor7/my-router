#pragma once
#include "Utils.h"
#include "string.h"
#include "RoutingTable.h"
#include "RouterSocket.h"

class EventHandler
{
public:

	//************************************
	// Method:    EventHandler
	// FullName:  EventHandler::EventHandler
	// Access:    public 
	// Returns:   
	// Qualifier: Constructor
	// Parameter: RoutingTable * router_table
	//************************************
	EventHandler(RoutingTable* router_table, RouterEntry* routers, fd_set* active, fd_set* read, fd_set* write,
				 vector<Subnet*>* subnets);
	
	//************************************
	// Method:    ~EventHandler
	// FullName:  EventHandler::~EventHandler
	// Access:    public 
	// Returns:   
	// Qualifier: Destructor
	// Parameter: void
	//************************************
	~EventHandler();

	//Enumerator of the possible events.
	enum RouterEvents
	{
		RT_EVENT_READ_CONFIG,
		RT_EVENT_TIMEOUT,
		RT_EVENT_DV_RECEIVED,
		RT_EVENT_SENDING_DV
	};

	//************************************
	// Method:    PrintEvent
	// FullName:  EventHandler::PrintEvent
	// Access:    public 
	// Returns:   std::string
	// Qualifier: Prints an event
	// Parameter: RouterEvents event
	//************************************
	string EventHandler::PrintEvent(RouterEvents event);

	//************************************
	// Method:    Handle
	// FullName:  EventHandler::Handle
	// Access:    public 
	// Returns:   Utils::ReturnStatus
	// Qualifier: Handles incoming data
	// Parameter: RouterEvents event
	// Parameter: void * data
	//************************************
	Utils::ReturnStatus EventHandler::Handle(RouterEvents event, void* data);

	//************************************
	// Method:    GetName
	// FullName:  EventHandler::GetName
	// Access:    public 
	// Returns:   std::string
	// Qualifier: Gets the router's name
	//************************************
	string GetName() { return m_name; };

	//************************************
	// Method:    SetName
	// FullName:  EventHandler::SetName
	// Access:    public 
	// Returns:   void
	// Qualifier: Set the router's name
	// Parameter: string name
	//************************************
	void SetName(string name) { m_name = name; };

	//************************************
	// Method:    SetNumOfRouters
	// FullName:  EventHandler::SetNumOfRouters
	// Access:    public 
	// Returns:   void
	// Qualifier: Set number of router
	// Parameter: int num
	//************************************
	void SetNumOfRouters(int num) { m_num_of_routers = num; };

	//Set my id member
	//************************************
	// Method:    SetMyId
	// FullName:  EventHandler::SetMyId
	// Access:    public 
	// Returns:   void
	// Qualifier: Set a pointer to the MyRouter in the router's array
	// Parameter: int id
	//************************************
	void SetMyId(int id) { m_my_entry = &(m_routers[id]); };

	//************************************
	// Method:    GetMyEntry
	// FullName:  EventHandler::GetMyEntry
	// Access:    public 
	// Returns:   RouterEntry*
	// Qualifier: Get router's entry
	//************************************
	RouterEntry* GetMyEntry() { return m_my_entry; };

	//************************************
	// Method:    AddRoutes
	// FullName:  EventHandler::AddRoutes
	// Access:    public 
	// Returns:   Utils::ReturnStatus
	// Qualifier: Adds route to routing table
	// Parameter: char name[MAX_ROUTER_NAME]
	// Parameter: Subnet * subnet_ptr
	//************************************
	Utils::ReturnStatus AddRoutes(RouterEntry* entry, Subnet* subnet_ptr);

	//Number of registered routers
	int m_num_of_routers; //TBD: change to private?
	
private:
	//************************************
	// Method:    IsNeighbor
	// FullName:  EventHandler::IsNeighbor
	// Access:    private 
	// Returns:   bool
	// Qualifier: Checks if two subnets are the same network
	// Parameter: Subnet * first_subnet_ptr
	// Parameter: Subnet * second_subnet_ptr
	//************************************
	bool IsNeighbor(Subnet* first_subnet_ptr, Subnet* second_subnet_ptr);
	//Routing table
	RoutingTable* m_table;

	//Router entries for routers
	RouterEntry* m_routers;
	RouterEntry* m_my_entry;
	
	//Router's subnets
	vector<Subnet*>* m_router_subnets;

	//Name of router
	string m_name;

	//Sets of sockets - used by select function in MyRouter
	fd_set* m_active_fd_set;
	fd_set* m_read_fd_set;
	fd_set* m_write_fd_set;
};
