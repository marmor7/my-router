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
	return m_name;
}

void MyRouter::SetName( string name )
{
	this->m_handler->setName(name);
	this->m_name = name;
}

Utils::ReturnStatus MyRouter::AddRouter(char name[MAX_ROUTER_NAME], in_addr* address, short port){
	IF_DEBUG(ALL){
		cout << "addRouter: " << name << ", ..." << 
			address->S_un.S_un_b.s_b1 << ":" << port << endl;
	}

	memcpy(this->m_routers[m_num_of_routers].name, name, MAX_ROUTER_NAME);
	this->m_routers[m_num_of_routers].address = *address;
	this->m_routers[m_num_of_routers].port = port;
	this->m_num_of_routers++;

	//TBD: is he a neighbour?
	this->m_routers[m_num_of_routers].neighbour = true;

	return Utils::STATUS_OK;
}

Utils::ReturnStatus MyRouter::AddRoute(char name[MAX_ROUTER_NAME], in_addr* ip_array, int num){
	IF_DEBUG(ALL){
		cout << "addRoute: " << name << endl;
	}

	//TBD: check if router is a neighbour and add it's details

	this->m_handler->addRoutes(name, ip_array, num);

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