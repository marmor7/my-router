#include "stdafx.h"
#include "EventHandler.h"

EventHandler::EventHandler(RoutingTable* router_table, RouterEntry* routers, 
						   fd_set* active, fd_set* read, fd_set* write)
{
	this->m_table = router_table;
	this->m_routers = routers;
	this->m_num_of_routers = 0;
	this->m_active_fd_set = active;
	this->m_read_fd_set = read;
	this->m_write_fd_set = write;
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

	int len = 0;

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
			cout << "Handle: sending my DV to neighbors" << endl;
		}
		MyRIPMessage msg;
		this->m_table->GetDV(&msg);
		for (int i=0; i < m_num_of_routers; i++){
			m_routers[i].msg_len = len;
			memcpy(m_routers[i].msg, &msg, len);
			FD_SET (m_routers[i].socketId, m_write_fd_set);
		}
		break;
	case RT_EVENT_TIMEOUT:
	case RT_EVENT_DV_RECEIVED:
		IF_DEBUG(TRACE){
			cout << "Got an event!!! Not doing anything yet..." << endl;
		}
		break;
	default:
		IF_DEBUG(ERROR){
			cout << "ERROR: got a weird event!!! Don't know what to do" << endl;
		}
	}

	return Utils::STATUS_OK;
}

Utils::ReturnStatus EventHandler::AddRoutes( char name[MAX_ROUTER_NAME], vector<Subnet*>* subnets_vector_ptr )
{
	for (vector<Subnet*>::iterator it = subnets_vector_ptr->begin();
		it != subnets_vector_ptr->end();
		++it)
	{
		//TBD: Check if IP and mask equals the router's subnet.
		//If yes, updates the m_routers table (turn on the neighbor bit only)		
		RoutingTable::AddRoute(name, *it);
	}

	return Utils::STATUS_OK;
}