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
	EventHandler(RoutingTable* router_table);
	
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
	
private:
	//Routing table
	RoutingTable* m_table;

	//Router entries for routers
	RouterEntry m_routers[NUM_OF_ROUTERS];

	//Num of registered routers
	int m_num_of_routers;
};
