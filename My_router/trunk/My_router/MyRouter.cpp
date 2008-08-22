#include "stdafx.h"
#include "MyRouter.h"
#include "EventHandler.h"
#include "RoutingTable.h"

MyRouter::~MyRouter()
{
	delete (this->m_handler);
	delete (this->m_routers);
	delete (this->m_table);
}

MyRouter::MyRouter( string name ) : m_name(name), m_num_of_routers(0), m_router_port(0)
{
	
	this->m_table = new RoutingTable();
	this->m_my_router_subnets = new vector<Subnet*>();
	this->m_handler = new EventHandler(m_table, m_routers, 
		&m_active_fd_set, &m_read_fd_set, &m_write_fd_set, m_my_router_subnets);
	this->m_routers = new RouterEntry[NUM_OF_ROUTERS];
}

string MyRouter::GetName()
{
	return m_name;
}

void MyRouter::SetName( string name )
{
	this->m_handler->SetName(name);
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
	this->m_handler->m_num_of_routers++;

	//TBD: is he a neighbour?
	this->m_routers[m_num_of_routers].neighbour = true;

	return Utils::STATUS_OK;
}

Utils::ReturnStatus MyRouter::AddRoute( char name[MAX_ROUTER_NAME], Subnet* subnet_ptr )
{
	IF_DEBUG(ALL)
	{
		cout << "AddRoute " << name << endl;
	}

	this->m_handler->AddRoutes(name, subnet_ptr);

	return Utils::STATUS_OK;
}

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
	for (i = 0; i <= m_max_fd; i++)
		if (FD_ISSET(i, &set))
			cout << i << ", ";
	cout << endl;
	for (i = 0; i < m_num_of_routers; i++)
		if (FD_ISSET(m_sockets[i], &set))
			cout << i << ": "<< m_sockets[i] << ", ";
	cout << "end." << endl;
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

	m_handler->Handle(EventHandler::RT_EVENT_READ_CONFIG, (void *)this->m_routers);

	displaySet("Read", m_read_fd_set);
	displaySet("Write", m_write_fd_set);
	displaySet("Active", m_active_fd_set);

	/*
	for (i = 0; i < m_num_of_routers; i++)
	{
		m_sockets[i] = m_routers[i].socketId;
		if (m_sockets[i] > m_max_fd)
			m_max_fd = m_sockets[i] + 1;
	} */

	m_my_fd = RouterSocket::GetRouterSocketDescriptor();
	
	FD_SET(m_my_fd, &m_active_fd_set);
	//TBD? FD_SET(m_max_fd, &m_write_fd_set);

	displaySet("Read", m_read_fd_set);
	displaySet("Write", m_write_fd_set);

	this->m_my_entry = this->m_handler->GetMyEntry();

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
		
		res = select(m_max_fd, &m_read_fd_set, &m_write_fd_set, NULL, &timeout);
		
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
		for (i = 0; i < m_num_of_routers; ++i)
			if (FD_ISSET(m_sockets[i], &m_write_fd_set))
			{
				IF_DEBUG(TRACE)
					cout << "writing to socket " << m_sockets[i] << endl;
				if (this->m_routers[i].msg_len > 0)
				{
					//TBD: Handle return status
					RouterSocket::SocketSend(RouterSocket::GetRouterSocketDescriptor(),		//Out sd
											 this->m_routers[i].msg_len,					//Length of data
											 this->m_routers[i].msg,						//Message to send
											 m_routers[i]);									//Router entry
				}
				if (this->m_routers[i].msg_len <= 0)
				{
					FD_CLR(m_sockets[i], &m_write_fd_set);
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

			RouterSocket::SocketReceive(m_my_fd,					//Socket descriptor
										this->m_my_entry->msg,		//In buffer
										this->m_my_entry->msg_len,	//Buffer length
										&sender);					//Sender struct containing sender data
			
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