#include "stdafx.h"
#include "EventHandler.h"

EventHandler::EventHandler(RoutingTable* router_table, RouterEntry* routers, 
						   fd_set* active, fd_set* read, fd_set* write,vector<Subnet*>* subnets)
{
	this->m_table = router_table;
	this->m_routers = routers;
	this->m_num_of_routers = 0;
	this->m_active_fd_set = active;
	this->m_read_fd_set = read;
	this->m_write_fd_set = write;
	this->m_router_subnets = subnets;
	this->m_my_entry = 0;
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

		RouterSocket::SocketInit();

		IF_DEBUG(TRACE)
		{
			cout << "Return value from socket init is: " << RouterSocket::GetRouterSocketDescriptor() << endl;
		}


		//TBD: RoutingTable::
		for (int i=0; i < m_num_of_routers; i++)
		{
			/*
			RouterSocket::SocketEstablish(&m_routers[i]);
			IF_DEBUG(TRACE)
			{
				cout << "socket to " << m_routers[i].name << 
					" established on " << m_routers[i].socketId << endl;
			} 
			*/
			if (m_name.compare(m_routers[i].name) == 0)
				m_my_entry = &(m_routers[i]);
		}

		/* NO BREAK NEEDED */
	case RT_EVENT_SENDING_DV:
		IF_DEBUG(TRACE)
		{
			cout << "Handle: sending my DV to neighbors" << endl;
		}

		MyRIPMessage msg;
		memset(&msg, 0, sizeof(msg));

		//Set all known fields:
		msg.protocolID = htons(PROTOCOL_ID);
		memcpy(msg.SenderName, m_my_entry->name, MAX_SENDER_NAME);

		//Let routing table set it's related fields:
		this->m_table->GetDV(&msg);
		for (int i=0; i < m_num_of_routers; i++)
		{
			//Set receiver specific fields:
			memcpy(msg.ReceiverName, m_routers[i].name, MAX_SENDER_NAME);
			/*TBD:
			Subnet subnet;
				this->m_table->GetRouterSubnet(&subnet);
				msg.ConnectingNETMYIPSubnet = subnet.address;
				msg.ConnectingNETMYIPMask   = subnet.mask;
			*/
			IF_DEBUG(TRACE)
			{
				if (i==0)
					Utils::PrintMsg(&msg);
			}

			memcpy(m_routers[i].msg, &msg, len);
			//TBD? FD_SET (m_routers[i].socketId, m_write_fd_set);
		}
		break;
	case RT_EVENT_TIMEOUT:
	case RT_EVENT_DV_RECEIVED:
		IF_DEBUG(TRACE)
		{
			cout << "Got an event!!! Not doing anything yet..." << endl;
		}
		break;
	default:
		IF_DEBUG(ERROR)
		{
			cout << "ERROR: got a weird event!!! Don't know what to do" << endl;
		}
	}

	return Utils::STATUS_OK;
}

Utils::ReturnStatus EventHandler::AddRoutes(RouterEntry* entry, Subnet* subnet_ptr )
{
	for (vector<Subnet*>::iterator it = this->m_router_subnets->begin();
		it != this->m_router_subnets->end();
		++it)
	{
		if (this->IsNeighbor(*it, subnet_ptr))
		{
			IF_DEBUG(TRACE)
			{
				cout << "Found neighbor: " << entry->name << endl;	
			}
			RoutingTable::AddRoute(entry->name, entry->address, entry->port, subnet_ptr);
		}
	}
	//TBD: Check if IP and mask equals the router's subnet.
	//If yes, updates the m_routers table (turn on the neighbor bit only)		

	return Utils::STATUS_OK;
}

bool EventHandler::IsNeighbor( Subnet* first_subnet_ptr, Subnet* second_subnet_ptr )
{
	unsigned int mask, first_subnet_address, second_subnet_address;
	mask = 0xFFFFFFFF;
	IF_DEBUG(ALL)
	{
		cout << "Comparing 2 subnets:" << endl;
		cout << "First subnet: " << inet_ntoa(first_subnet_ptr->address) << 
				" with mask " << first_subnet_ptr->mask << endl;
		cout << "Second subnet: " << inet_ntoa(second_subnet_ptr->address) << 
				" with mask " << second_subnet_ptr->mask << endl;
	}
	
	//If the 2 masks are different always return false
	if (first_subnet_ptr->mask != second_subnet_ptr->mask)
	{
		IF_DEBUG(ALL)
		{
			cout << "Mask are not equal, returning false." << endl;
		}

		return false;
	}
	else
	{
		mask = mask << (32 - first_subnet_ptr->mask);

		first_subnet_address = htonl(first_subnet_ptr->address.S_un.S_addr);
		second_subnet_address = htonl(second_subnet_ptr->address.S_un.S_addr);

		first_subnet_address = first_subnet_address & mask;
		second_subnet_address = second_subnet_address & mask;
		
		if (first_subnet_address == second_subnet_address)
		{
			IF_DEBUG(ALL)
			{
				cout << "Subnets equals, returning true." << endl;
			}

			return true;
		}
		else
		{
			IF_DEBUG(ALL)
			{
				cout << "Subnets are not equal, returning false." << endl;
			}
			return false;
		}
	}
}