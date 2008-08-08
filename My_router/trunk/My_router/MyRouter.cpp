#include "stdafx.h"
#include "MyRouter.h"

MyRouter::~MyRouter()
{
}

MyRouter::MyRouter( string name )
{
	this->m_name = name;
	this->table = new RoutingTable();
	this->handler = new EventHandler(table);
	this->routers = new RouterEntry[NUM_OF_ROUTERS];
	this->numOfRouters = 0;
}

string MyRouter::GetName()
{
	return this->m_name;
}

void MyRouter::SetName( string name )
{
	this->m_name = name;
}

Utils::ReturnStatus MyRouter::addRouter(char* name, in_addr* address, short port){
	memcpy(this->routers[numOfRouters].name, name, MAX_ROUTER_NAME);
	memcpy(this->routers[numOfRouters].address, address, sizeof(in_addr));
	this->routers[numOfRouters].port = port;

	return Utils::STATUS_OK;
}

void MyRouter::run()
{
	handler->handle(EventHandler::RT_EVENT_READ_CONFIG, (void *)this->routers);

	while (true)
	{
		break;
	}

	delete(handler);
}