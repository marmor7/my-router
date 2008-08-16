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

	string getName() { return m_name; };

	void setName(string name) { m_name = name; };

	void setNumOfRouters(int num) { m_num_of_routers = num; };

	Utils::ReturnStatus AddRoutes(char name[MAX_ROUTER_NAME], Subnet* subnet_ptr);

	//Num of registered routers
	int m_num_of_routers; //TBD: change to private?
	
private:
	//Routing table
	RoutingTable* m_table;

	//Router entries for routers
	RouterEntry* m_routers;
	
	//Router's subnets
	vector<Subnet*>* m_router_subnets;

	//Name of router
	string m_name;

	//Sets of sockets - used by select function in MyRouter
	fd_set* m_active_fd_set;
	fd_set* m_read_fd_set;
	fd_set* m_write_fd_set;
};
