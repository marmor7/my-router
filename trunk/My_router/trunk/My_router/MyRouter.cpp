#include "stdafx.h"
#include "MyRouter.h"
#include "EventHandler.h"
#include "RoutingTable.h"

MyRouter::~MyRouter()
{
}

MyRouter::MyRouter( string name )
{
	this->m_name = name;
	this->m_table = new RoutingTable();
	this->m_handler = new EventHandler(m_table);
	this->m_routers = new RouterEntry[NUM_OF_ROUTERS];
	this->m_num_of_routers = 0;
}

string MyRouter::GetName()
{
	return this->m_name;
}

void MyRouter::SetName( string name )
{
	this->m_name = name;
}

Utils::ReturnStatus MyRouter::AddRouter(char* name, in_addr* address, short port){
	memcpy(this->m_routers[m_num_of_routers].name, name, MAX_ROUTER_NAME);
	memcpy(this->m_routers[m_num_of_routers].address, address, sizeof(in_addr));
	this->m_routers[m_num_of_routers].port = port;

	return Utils::STATUS_OK;
}

void MyRouter::Run()
{
	m_handler->Handle(EventHandler::RT_EVENT_READ_CONFIG, (void *)this->m_routers);

	while (true)
	{
		break;
	}

	delete(m_handler);
}