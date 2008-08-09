#include "EventHandler.h"

EventHandler::EventHandler(RoutingTable* router_table)
{
	this->m_table = router_table;
	this->m_num_of_routers = 0;
}

EventHandler::~EventHandler(void)
{
}

string EventHandler::PrintEvent(RouterEvents event)
{
	switch (event)
	{
	case RT_EVENT_READ_CONFIG : return "read config file";
	case RT_EVENT_TIMEOUT     : return "timeout!";
	case RT_EVENT_DV_RECEIVED : return "DV received";
	case RT_EVENT_SENDING_DV  : return "sending DV";
	default : return "Unknown event";
	}
	return "";
}

Utils::ReturnStatus EventHandler::Handle(RouterEvents event, void* data)
{
	cout << this->m_name << " MYRIP Event: " << PrintEvent(event) << endl;

	switch (event)
	{
	case RT_EVENT_READ_CONFIG:
		this->m_routers = (RouterEntry* )data;
		//TBD: RoutingTable::
		for (int i=0; i < m_num_of_routers; i++){
			RouterSocket::SocketEstablish(&m_routers[i]);
		}

		/* NO BREAK NEEDED */
	case RT_EVENT_SENDING_DV:
		IF_DEBUG(TRACE){
			cout << "Sending my DV to neighbours" << endl;
		}
		break;
	case RT_EVENT_TIMEOUT:
	case RT_EVENT_DV_RECEIVED:
		IF_DEBUG(TRACE){
			cout << "got an event!!! not doing enything yet..." << endl;
		}
		break;
	default:
		IF_DEBUG(IMPORTANT){
			cout << "got a wierd event!!! don't know what to do" << endl;
		}
	}

	return Utils::STATUS_OK;
}

Utils::ReturnStatus EventHandler::addRoutes(char name[MAX_ROUTER_NAME], in_addr* ip_array, int num){
	for (int i=0; i < num; i++){
		RoutingTable::addRoute(name, &(ip_array[i]));
	}

	return Utils::STATUS_OK;
}