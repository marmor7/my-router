#pragma once
#include "Utils.h"
#include "string.h"

class EventHandler
{
public:
	EventHandler(void);
	~EventHandler(void);

	//Enumerator of the possible events.
	enum RouterEvents
	{
		RT_EVENT_READ_CONFIG,
		RT_EVENT_TIMEOUT,
		RT_EVENT_DV_RECEIVED,
		RT_EVENT_SENDING_DV
	};

	string EventHandler::printEvent(RouterEvents event);

	Utils::ReturnStatus EventHandler::handle(RouterEvents event);
};
