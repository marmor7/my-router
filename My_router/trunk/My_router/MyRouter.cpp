#include "stdafx.h"
#include "MyRouter.h"
#include "EventHandler.h"
#include "RoutingTable.h"

MyRouter::MyRouter() : m_name(""), m_num_of_routers(0), m_router_port(0)
{
	this->m_table = new RoutingTable();
	this->m_routers = new RouterEntry[NUM_OF_ROUTERS];
	this->m_handler = new EventHandler(m_table, m_routers, 
		&m_active_fd_set, &m_read_fd_set, &m_write_fd_set);
}

MyRouter::~MyRouter()
{
	delete (this->m_handler);
	delete (this->m_routers);
	delete (this->m_table);
}

MyRouter::MyRouter( string name ) : m_name(name), m_num_of_routers(0), m_router_port(0)
{
	
	this->m_table = new RoutingTable();
	this->m_handler = new EventHandler(m_table, m_routers,
		&m_active_fd_set, &m_read_fd_set, &m_write_fd_set);
	this->m_routers = new RouterEntry[NUM_OF_ROUTERS];
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

Utils::ReturnStatus MyRouter::AddRoute( char name[MAX_ROUTER_NAME], vector<Subnet*>* subnets_vector_ptr )
{
	IF_DEBUG(ALL)
	{
		cout << "AddRoute: " << name << endl;
	}

	//TBD: check if router is a neighbour and add it's details

	this->m_handler->AddRoutes(name, subnets_vector_ptr);

	return Utils::STATUS_OK;
}

void MyRouter::initSets()
{
	FD_ZERO(&m_write_fd_set);
	FD_ZERO(&m_active_fd_set);
	FD_ZERO(&m_read_fd_set);
}

void MyRouter::Run()
{
	int i = 0, res = 0;
	timeval timeout = {0};
	//const timeval timeout = {30, 0}; //TMP
	initSets();
	srand((int) time(NULL));

	m_handler->Handle(EventHandler::RT_EVENT_READ_CONFIG, (void *)this->m_routers);

	while (true)
	{
		IF_DEBUG(ALL)
		{
			cout << "while loop..." << endl;
		}

		//Set a random timeout in range NIM ... MAX
		timeout.tv_sec = rand() % (TIMEOUT_SEC_MAX - TIMEOUT_SEC_MIN) + TIMEOUT_SEC_MIN;

		m_read_fd_set = m_active_fd_set;
		
		res = select (FD_SETSIZE, &m_read_fd_set, &m_write_fd_set, NULL, &timeout);
		if (res < 0)
		{
			IF_DEBUG(ERROR){
				cout << "select error " << res << endl;
			}
			perror ("select error ");
			cout << WSAGetLastError() << endl;//TMP
			exit (EXIT_FAILURE);
		}

		/* write to all ready sockets that have something in their buffer */
		for (i = 1; i < FD_SETSIZE; ++i)
			if (FD_ISSET(i, &m_write_fd_set))
			{
				if (this->m_routers[i].msg_len > 0)
				{
					RouterSocket::SocketSend(i, this->m_routers[i].msg_len, this->m_routers[i].msg);
				}

				if (this->m_routers[i].msg_len <= 0)
				{
					FD_CLR(i, &m_write_fd_set);
				}
			}

		/* read from all ready sockets */
		/*for (i = 1; i < FD_SETSIZE; ++i)
			if (FD_ISSET (i, &read_fd_set))
			{
				// recv data from a client 
				res = recv_msg (i, clients[i].buf, &clients[i].len, &msg_handle);
				if (res < 0)
				{
					close (i);
					client_disconnect(i);
				}
				}
			}*/
	}
}

void MyRouter::SetRoutersIpAndPort( string ip, short port )
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
	this->m_my_router_subnets.push_back(subnet);
}