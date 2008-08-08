#include "StdAfx.h"
#include "EventHandler.h"

EventHandler::EventHandler(void)
{
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

Utils::ReturnStatus EventHandler::handle(RouterEvents event)
{
	IF_DEBUG(IMPORTANT){
		cout << "handling event: " << printEvent(event) << endl;
	}

	switch (event)
	{
	case RT_EVENT_READ_CONFIG:
	case RT_EVENT_TIMEOUT:
	case RT_EVENT_DV_RECEIVED:
	case RT_EVENT_SENDING_DV:
		cout << "got an event!!! not doing enything yet..." << endl;
		break;
	default:
		cout << "got a wierd event!!! don't know what to do" << endl;
	}

	return Utils::STATUS_OK;
}