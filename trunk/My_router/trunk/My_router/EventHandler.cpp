#include "StdAfx.h"
#include "EventHandler.h"

EventHandler::EventHandler(RoutingTable* router_table)
{
	this->table = router_table;
	this->numOfRouters = 0;
}

EventHandler::~EventHandler(void)
{
}

string EventHandler::printEvent(RouterEvents event)
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

Utils::ReturnStatus EventHandler::handle(RouterEvents event, void* data)
{
	IF_DEBUG(IMPORTANT){
		cout << "handling event: " << printEvent(event) << endl;
	}

	switch (event)
	{
	case RT_EVENT_READ_CONFIG:
		this->numOfRouters = 0;
		memcpy(this->routers, data, NUM_OF_ROUTERS*sizeof(RouterEntry));
		RouterSocket::socketEstablish();

		/* NO BREAK NEEDED */
	case RT_EVENT_SENDING_DV:
		IF_DEBUG(TRACE){
			cout << "Sending my DV to neighbours" << endl;
		}
		break;
	case RT_EVENT_TIMEOUT:
	case RT_EVENT_DV_RECEIVED:
		cout << "got an event!!! not doing enything yet..." << endl;
		break;
	default:
		IF_DEBUG(IMPORTANT){
			cout << "got a wierd event!!! don't know what to do" << endl;
		}
	}

	return Utils::STATUS_OK;
}