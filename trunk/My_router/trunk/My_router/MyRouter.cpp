#include "stdafx.h"
#include "MyRouter.h"
#include "RoutingTable.h"
#include "RouterSocket.h"

MyRouter::~MyRouter()
{
	//TMP delete (this->m_handler);
	delete (this->m_routers);
	delete (this->m_table);
}

MyRouter::MyRouter( string name ) : m_name(name), m_num_of_routers(0), m_router_port(0)
{
	
	this->m_table = new RoutingTable();
	this->m_my_router_subnets = new vector<Subnet*>();
	//TMP this->m_handler = new EventHandler(m_table, m_routers, 
	//	&m_active_fd_set, &m_read_fd_set, &m_write_fd_set, m_my_router_subnets);
	this->m_routers = new RouterEntry[NUM_OF_ROUTERS];
}

string MyRouter::GetName()
{
	return m_name;
}

void MyRouter::SetName( string name )
{
	//TMP this->m_handler->SetName(name);
	this->m_name = name;
}

Utils::ReturnStatus MyRouter::AddRouter(char name[MAX_ROUTER_NAME], in_addr* address, unsigned short port){
	IF_DEBUG(ALL)
	{
		cout << "AddRouter: " << name << ", ..." << 
			(int)address->S_un.S_un_b.s_b4 << " : " << port << endl;
	}

	memcpy(this->m_routers[m_num_of_routers].name, name, MAX_ROUTER_NAME);
	this->m_routers[m_num_of_routers].address = *address;
	this->m_routers[m_num_of_routers].port = port;
	this->m_num_of_routers++;
	//TMP this->m_handler->m_num_of_routers++;

	//TBD: is he a neighbour?
	this->m_routers[m_num_of_routers].neighbour = true;

	return Utils::STATUS_OK;
}

//TMP
//Utils::ReturnStatus MyRouter::AddRoute( char name[MAX_ROUTER_NAME], Subnet* subnet_ptr )
//{
//	IF_DEBUG(ALL)
//		cout << "AddRoute " << name << endl;
//
//	for (int i = 0; i < m_num_of_routers; i++)
//	{
//		if (strncmp(name, m_routers[i].name, MAX_ROUTER_NAME) == 0){
//			this->m_handler->AddRoutes(&m_routers[i], subnet_ptr);
//			break;
//		}
//	}
//
//	return Utils::STATUS_OK;
//}

void MyRouter::InitSets()
{
	FD_ZERO(&m_write_fd_set);
	FD_ZERO(&m_active_fd_set);
	FD_ZERO(&m_read_fd_set);
}

void MyRouter::displaySet(string title, fd_set & set)
{
	int i;
	cout << title << " set is: ";
	for (i = 0; i <= m_my_fd; i++)
		if (FD_ISSET(i, &set))
			cout << i << ", ";
	cout << endl;
}

void MyRouter::Run()
{
	int i = 0, res = 0;
	m_max_fd = 10000;
	int myId = -1;
	timeval timeout = {30, 0};
	InitSets();
	srand((int) time(NULL));

	displaySet("Read", m_read_fd_set);
	displaySet("Write", m_write_fd_set);
	displaySet("Active", m_active_fd_set);

	//Some sizes checks:
	assert(sizeof(int) == 4);
	assert(sizeof(long) == 4);
	assert(sizeof(short) == 2);
	assert(sizeof(byte) == 1);

	Handle(MyRouter::RT_EVENT_READ_CONFIG, (void *)this->m_routers);

	/*
	for (i = 0; i < m_num_of_routers; i++)
	{
		m_sockets[i] = m_routers[i].socketId;
		if (m_sockets[i] > m_max_fd)
			m_max_fd = m_sockets[i] + 1;
	} */

	m_my_fd = RouterSocket::GetRouterSocketDescriptor();

	InitSets();

	displaySet("Read", m_read_fd_set);
	displaySet("Write", m_write_fd_set);
	displaySet("Active", m_active_fd_set);
	
	FD_SET(m_my_fd, &m_active_fd_set);
	//TBD? FD_SET(m_max_fd, &m_write_fd_set);

	displaySet("Read", m_read_fd_set);
	displaySet("Write", m_write_fd_set);

	this->m_my_entry = GetMyEntry();

	while (true)
	{
		IF_DEBUG(ALL)
		{
			cout << "while loop..." << endl;
		}

		//Set a random timeout in range NIM ... MAX
		timeout.tv_sec = 5;//TBD: rand() % (TIMEOUT_SEC_MAX - TIMEOUT_SEC_MIN) + TIMEOUT_SEC_MIN;

		FD_CLR(0, &m_active_fd_set);
		m_read_fd_set = m_active_fd_set;

		displaySet("Read", m_read_fd_set);
		displaySet("Write", m_write_fd_set);
		
		res = select(m_my_fd+1, &m_read_fd_set, &m_write_fd_set, NULL, &timeout);
		
		IF_DEBUG(TRACE)
		{
			cout << "select returned " << res << endl;
		}

		if (res < 0)
		{
			IF_DEBUG(ERROR)
			{
				cout << "select error " << res << endl;
			}
			perror ("select error ");
			cout << WSAGetLastError() << endl;//TMP
			exit (EXIT_FAILURE);
		}

		displaySet("Read", m_read_fd_set);
		displaySet("Write", m_write_fd_set);

		/* write to all ready sockets that have something in their buffer */
		if (FD_ISSET(m_my_fd, &m_write_fd_set))
		{
			IF_DEBUG(TRACE)
				cout << "writing to socket " << m_my_fd << endl;
			if (m_out_buf.len > 0)
			{
				//TBD: Handle return status
				RouterSocket::SocketSend(RouterSocket::GetRouterSocketDescriptor(),	//Out sd
										 m_out_buf.len,								//Length of data
										 m_out_buf.msg,								//Message to send
										 *m_my_entry);								//Router entry
			}
			if (m_out_buf.len <= 0)
			{
				FD_CLR(m_my_fd, &m_write_fd_set);
			}
		}

		/* read from all ready sockets */
		if (FD_ISSET (m_my_fd, &m_read_fd_set))
		{
			sockaddr_in sender;
			IF_DEBUG(TRACE)
			{
				cout << "reading from socket " << m_my_fd << endl;
			}

			RouterSocket::SocketReceive(m_my_fd,		//Socket descriptor
										m_in_buf.msg,	//In buffer
										m_in_buf.len,	//Buffer length
										&sender);		//Sender struct containing sender data
			
			FD_CLR(m_my_fd, &m_read_fd_set);
		}
	}

	cout << "While loop exit" << endl;
}

void MyRouter::SetRoutersIpAndPort( string ip,unsigned short port )
{
	long ip_long;
	struct in_addr s;

	memset(&this->m_router_ip, 0, sizeof(struct sockaddr_in));
	memset(&s, 0, sizeof(struct in_addr));

	this->m_router_ip.sin_family = AF_INET;
	this->m_router_ip.sin_port = htons(port);
	ip_long = inet_addr(ip.c_str());

	assert(ip_long != INADDR_ANY);

	s.S_un.S_addr = ip_long;
	this->m_router_ip.sin_addr = s;
	this->m_router_port = port;

	IF_DEBUG(TRACE)
	{
		cout << "MyRouter's IP is : " << (unsigned int) this->m_router_ip.sin_addr.S_un.S_un_b.s_b1 << "."
									  << (unsigned int) this->m_router_ip.sin_addr.S_un.S_un_b.s_b2 << "."
									  << (unsigned int) this->m_router_ip.sin_addr.S_un.S_un_b.s_b3 << "."
									  << (unsigned int) this->m_router_ip.sin_addr.S_un.S_un_b.s_b4 << endl;
		cout << "MyRouter's port is: " << this->m_router_port << endl; 
	}
}

void MyRouter::AddSubnet( Subnet* subnet )
{
	this->m_my_router_subnets->push_back(subnet);
}


string MyRouter::PrintEvent(RouterEvents event)
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

Utils::ReturnStatus MyRouter::Handle(RouterEvents event, void* data)
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

			memcpy(m_out_buf.msg, &msg, len);
			m_out_buf.len = sizeof(msg);
			FD_SET (m_my_fd, &m_write_fd_set);
		}
		break;
	case RT_EVENT_TIMEOUT:
	case RT_EVENT_DV_RECEIVED:
		IF_DEBUG(TRACE)
			cout << "Got an event!!! Not doing anything yet..." << endl;
		break;
	default:
		IF_DEBUG(ERROR)
			cout << "ERROR: got a weird event!!! Don't know what to do" << endl;
	}

	return Utils::STATUS_OK;
}

Utils::ReturnStatus MyRouter::AddRoute(char name[MAX_ROUTER_NAME], Subnet* subnet_ptr )
{
	int i;
	for (i = 0; i < m_num_of_routers; i++)
	{
		if (strncmp(name, m_routers[i].name, MAX_ROUTER_NAME) == 0)
			break;
	}
	if (i >= m_num_of_routers){
		IF_DEBUG(ERROR)
			cout << "Error: " << name << " wasn't found in table" << endl;
		return Utils::STATUS_BAD_PARAMETER;
	}

	for (vector<Subnet*>::iterator it = this->m_my_router_subnets->begin();
		it != this->m_my_router_subnets->end();
		++it)
	{
		if (this->IsNeighbor(*it, subnet_ptr))
		{
			IF_DEBUG(TRACE)
			{
				cout << "Found neighbor: " << m_routers[i].name << endl;	
			}
			RoutingTable::AddRoute(m_routers[i].name, m_routers[i].address, 
									m_routers[i].port, subnet_ptr);
		}
	}
	//TBD: Check if IP and mask equals the router's subnet.
	//If yes, updates the m_routers table (turn on the neighbor bit only)		

	return Utils::STATUS_OK;
}

bool MyRouter::IsNeighbor( Subnet* first_subnet_ptr, Subnet* second_subnet_ptr )
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