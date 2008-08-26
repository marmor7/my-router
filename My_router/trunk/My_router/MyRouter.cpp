#include "stdafx.h"
#include "MyRouter.h"
#include "RoutingTable.h"
#include "RouterSocket.h"

MyRouter::~MyRouter()
{
	delete (this->m_routers);
	delete (this->m_table);
}

MyRouter::MyRouter( string name ) : m_num_of_routers(0), m_router_port(0)
{
	memcpy(m_name, name.c_str(), MAX_ROUTER_NAME);
	this->m_table = new RoutingTable();
	this->m_my_router_subnets = new vector<Subnet*>();
	this->m_routers = new RouterEntry[NUM_OF_ROUTERS];
}

char* MyRouter::GetName()
{
	return m_name;
}

void MyRouter::SetName( string name )
{
	memcpy(m_name, name.c_str(), MAX_ROUTER_NAME);
}

Utils::ReturnStatus MyRouter::AddRouter(char name[MAX_ROUTER_NAME], in_addr* address, unsigned short port){
	IF_DEBUG(ALL)
	{
		cout << "AddRouter: " << name << ", ..." << 
			(int)address->S_un.S_un_b.s_b4 << " : " << port << endl;
	}

	memcpy(this->m_routers[m_num_of_routers].name, name, MAX_ROUTER_NAME);
	m_routers[m_num_of_routers].address = *address;
	m_routers[m_num_of_routers].port = port;
	m_routers[m_num_of_routers].reachable = false;

	if (strncmp(name, m_name, MAX_ROUTER_NAME) == 0){
		m_my_entry = m_routers[m_num_of_routers];
	}

	this->m_num_of_routers++;

	return Utils::STATUS_OK;
}

void MyRouter::InitSets()
{
	FD_ZERO(&m_write_fd_set);
	FD_ZERO(&m_active_fd_set);
	FD_ZERO(&m_read_fd_set);
}

void MyRouter::DisplaySet(string title, fd_set & set)
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
	Utils::SocketReturnStatus socket_return_status;
	timeval timeout = {0, 0};
	timeval oldtime = {0, 0};
	time_t before = {0}; //TMP?
	time_t after = {0}; //TMP?
	srand((int) time(NULL));

	InitSets();
	IF_DEBUG(TRACE)
	{
		DisplaySet("Read", m_read_fd_set);
		DisplaySet("Write", m_write_fd_set);
		DisplaySet("Active", m_active_fd_set);
	}

	//Some sizes checks:
	assert(sizeof(int) == 4);
	assert(sizeof(long) == 4);
	assert(sizeof(short) == 2);
	assert(sizeof(byte) == 1);

	Handle(MyRouter::RT_EVENT_READ_CONFIG, null);
	
	for (i = 0; i < m_num_of_routers; i++)
	{
		//Set the initial fail timeout for each neighbor
		SET_TIMEOUT(m_routers[i].timeout, TIMEOUT_FAIL)
	}
	SET_TIMEOUT(m_my_entry.timeout, 0);

	m_my_fd = RouterSocket::GetRouterSocketDescriptor();

	FD_SET(m_my_fd, &m_active_fd_set);

	IF_DEBUG(TRACE)
	{
		DisplaySet("Read", m_read_fd_set);
		DisplaySet("Write", m_write_fd_set);
		DisplaySet("Active", m_active_fd_set);
	}

	while (true)
	{
		IF_DEBUG(ALL)
		{
			cout << "while loop..." << endl;
		}

		if (SET_CMP(m_my_entry.timeout, 0, 0))
		{
			//Set a random timeout in range NIM ... MAX
			SET_TIMEOUT(m_my_entry.timeout, 
						rand() % (TIMEOUT_SEND_MIN - TIMEOUT_SEND_MAX) + TIMEOUT_SEND_MIN);
		}

		//Set timeout to be the LOWEST timeout, se we'll wake up
		SET_TIMEOUT(timeout, m_my_entry.timeout.tv_sec);
		for (i=0; i < m_num_of_routers; i++)
		{
			if ((m_routers[i].reachable) && 
				(timeout.tv_sec > m_routers[i].timeout.tv_sec))
				timeout = m_routers[i].timeout;
		}
		oldtime = timeout;

		FD_CLR(0, &m_active_fd_set);
		m_read_fd_set = m_active_fd_set;

		IF_DEBUG(TRACE)
		{
			cout << "Timeout: " << timeout.tv_sec << ": " << timeout.tv_usec << endl;
			DisplaySet("Write", m_write_fd_set);
		}
		
		time(&before); //TMP?
		res = select(m_my_fd+1, &m_read_fd_set, &m_write_fd_set, NULL, &timeout);
		time(&after); //TMP?

		IF_DEBUG(TRACE){
			cout << "Timeout: " << timeout.tv_sec << ": " << timeout.tv_usec << endl;
			DisplaySet("Write", m_write_fd_set);
			cout << "select returned " << res << endl;
		}

		if (res < 0)
		{
			IF_DEBUG(ERROR)
				cout << "select error " << res << endl;
			perror ("select error ");
			cout << WSAGetLastError() << endl;//TMP
			exit (EXIT_FAILURE);
		}

		//TMP: check that timeout is modified on Linux, 
		//     and remove this if
		if (timeout.tv_sec > (after - before))
			timeout.tv_sec -= (long)(after - before);
		else
			SET_TIMEOUT(timeout, 0);

		//Set the difference to oldtime
		TIMERSUB(&oldtime, &timeout, &oldtime);
		for (i=0; i < m_num_of_routers; i++){
			if ((m_routers[i].timeout.tv_sec <= oldtime.tv_sec) && 
				(m_routers[i].reachable))
			{
				//Neighbor #i had not responded - assume down:
				SET_TIMEOUT(m_routers[i].timeout, 0);
				Handle(RT_EVENT_TIMEOUT, ((void *)&i));
			}
			else
			{
				//Reduce the timeout value by the difference
				TIMERSUB(&m_routers[i].timeout, &oldtime, &m_routers[i].timeout);
			}
		}

		if (m_my_entry.timeout.tv_sec <= oldtime.tv_sec)
		{
			//Neighbor #i had not responded - assume down:
			SET_TIMEOUT(m_my_entry.timeout, 0);
			Handle(RT_EVENT_SENDING_DV, null);
		}
		else{
			//Reduce the timeout value by the difference
			TIMERSUB(&m_my_entry.timeout, &oldtime, &m_my_entry.timeout);
		}

		/* write to all ready sockets that have something in their buffer */
		if (FD_ISSET(m_my_fd, &m_write_fd_set))
		{
			IF_DEBUG(TRACE)
				cout << "Writing to socket " << m_my_fd << endl;
			int i;
			//Send a msg to every neighbor with a waiting msg
			for (i=0; i < m_num_of_routers; i++){
				if ((m_routers[i].out.len > 0) &&
					(m_routers[i].reachable))
				{
					socket_return_status = RouterSocket::SocketSend(RouterSocket::GetRouterSocketDescriptor(),	//Out sd
																	m_routers[i].out.len,						//Length of data
																	m_routers[i].out.msg,						//Message to send
						   											m_routers[i]);								//Router entry
					
					if (socket_return_status != Utils::STATUS_SEND_OK)
					{
						cout << "Error sending on socket: " << RouterSocket::GetRouterSocketDescriptor() << endl;
					}
					else
					{
						if (m_routers[i].out.len >= sizeof(MyRIPMessage))
							m_routers[i].out.len = 0;
						else
							m_routers[i].out.len = sizeof(MyRIPMessage);
					}
				}
			}

			//If we've finished sending to everyone, clear write fd
			for (i=0; i < m_num_of_routers; i++)
			{
				if (m_routers[i].out.len > 0)
					break;
			}

			if (i >= m_num_of_routers)
				FD_CLR(m_my_fd, &m_write_fd_set);
		}

		/* Read from all ready sockets */
		if (FD_ISSET (m_my_fd, &m_read_fd_set))
		{
			sockaddr_in sender = {0};
			IF_DEBUG(TRACE)
			{
				cout << "Reading from socket " << m_my_fd << endl;
			}
			m_in_buf.len = SIZE_OF_RIP_MSG;

			//TBD: Handle return status
			socket_return_status = RouterSocket::SocketReceive(m_my_fd,		//Socket descriptor
										m_in_buf.msg,	//In buffer
										m_in_buf.len,	//Buffer length
										&sender);		//Sender struct containing sender data

			IF_DEBUG(TRACE)
				cout << "Size of message received: " << m_in_buf.len << 
						"(" << SIZE_OF_RIP_MSG << ")" << endl;

			//Error checking
			if ((socket_return_status == Utils::STATUS_RECIVE_OK) && //Received message was fine
				(m_in_buf.len == SIZE_OF_RIP_MSG))					 //Size of message is fine
			{
				MyRIPMessage* recieved_msg;
				recieved_msg = (MyRIPMessage *)m_in_buf.msg;
				Utils::net2hostMsg(recieved_msg); //Make the message readable

				IF_DEBUG(TRACE)
				{
					cout << "Sender name is: " << recieved_msg->SenderName << endl;
					cout << "Protocol is: " << recieved_msg->protocolID << endl;
				}

				//Handle only version 1 protocols:
				if (recieved_msg->protocolID == 1)
				{
					for (i=0; i < m_num_of_routers; i++)
					{
						if (strncmp(recieved_msg->SenderName, m_routers[i].name, MAX_ROUTER_NAME) == 0)
						{
							IF_DEBUG(TRACE)
							{
								cout << "Found the neighbor! " << i << endl;
							}

							SET_TIMEOUT(m_routers[i].timeout, TIMEOUT_FAIL);
							m_routers[i].reachable = true;

							Handle(RT_EVENT_DV_RECEIVED, (void *)&i);
						}
					}
				}
				
				//Clear SET
				FD_CLR(m_my_fd, &m_read_fd_set);
			}		
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


string MyRouter::PrintEvent(RouterEvents incoming_event)
{
	switch (incoming_event)
	{
	case RT_EVENT_READ_CONFIG : return "Read config file";
	case RT_EVENT_TIMEOUT     : return "Timeout!";
	case RT_EVENT_DV_RECEIVED : return "DV received";
	case RT_EVENT_SENDING_DV  : return "Sending DV";
	default : return "Unknown event";
	}
	return "";
}

Utils::ReturnStatus MyRouter::Handle(RouterEvents incoming_event, void* data)
{
	cout << this->m_name << " MYRIP Event: " << PrintEvent(incoming_event) << endl;

	int len = 0;
	bool printed = false;
	int rt = 0;
	MyRIPMessage* recieved_msg;

	switch (incoming_event)
	{
	case RT_EVENT_READ_CONFIG:
		RouterSocket::SocketInit(this->m_router_port);

		IF_DEBUG(TRACE)
			cout << "Return value from socket init is: " << RouterSocket::GetRouterSocketDescriptor() << endl;

		m_table->PrintDV();

		IF_DEBUG(TRACE)
			m_table->PrintMap();

		break;
	case RT_EVENT_SENDING_DV:
		IF_DEBUG(TRACE)
			cout << "Handle: sending my DV to neighbors" << endl;

		MyRIPMessage msg;
		memset(&msg, 0, sizeof(msg));

		//Set all known fields:
		msg.protocolID = PROTOCOL_ID;
		memcpy(msg.SenderName, m_name, MAX_SENDER_NAME);
		msg.length = SIZE_OF_RIP_MSG;

		//Let routing table set it's related fields:
		this->m_table->GetDV(&msg);
		for (int i=0; i < m_num_of_routers; i++)
		{
			if (!m_routers[i].reachable)
				continue;

			//Set receiver specific fields:
			memcpy(msg.ReceiverName, m_routers[i].name, MAX_SENDER_NAME);
			Subnet subnet = {0};

			//TBD: handle return value
			this->m_table->GetRouterSubnet(&m_routers[i], &subnet);
			msg.ConnectingNETMYIPSubnet = subnet.address.S_un.S_addr;
			msg.ConnectingNETMYIPMask   = subnet.mask;

			IF_DEBUG(TRACE)
			{
				if (!printed)
				{
					printed = true;
					Utils::PrintMsg(&msg);
				}
			}

			//Copy msg to router's out buffer in correct byte order
			Utils::host2netMsg(&msg);
			memcpy(m_routers[i].out.msg, &msg, SIZE_OF_RIP_MSG);
			Utils::net2hostMsg(&msg);
			m_routers[i].out.len = SIZE_OF_RIP_MSG;
		}

		FD_SET(m_my_fd, &m_write_fd_set);
		break;

	case RT_EVENT_TIMEOUT:
		rt = *((int *)data);

		IF_DEBUG(TRACE)
			cout << "Handle: Timeout on router " << m_routers[rt].name << endl;

		//Mark neighbor as down
		m_routers[rt].reachable = false;

		this->m_table->ReportDeadRouter(m_routers[rt].name);

		//Print the new DV
		this->m_table->PrintDV();

		IF_DEBUG(TRACE)
			m_table->PrintMap();
		break;

	case RT_EVENT_DV_RECEIVED:
		recieved_msg = (MyRIPMessage *)m_in_buf.msg;
		rt = *((int *)data);

		IF_DEBUG(TRACE)
		{
			cout << "Handle: DV received from router " << m_routers[rt].name << endl;
		}

		if (!m_routers[rt].reachable)
		{
			//A dead router is up!
			m_routers[rt].reachable = true;
		}

		IF_DEBUG(ALL)
		{
			in_addr temp;
			temp.S_un.S_addr = recieved_msg->ConnectingNETMYIPSubnet;

			cout << endl;
			cout << "Handle DV Received: Scanning incoming message for data."<< endl;
			cout << "Size of message: " << recieved_msg->length << endl;
			cout << "Protocol ID: " << recieved_msg->protocolID << endl;
			cout << "Connecting NET MYIP Subnet and mask are: " << inet_ntoa(temp) << "/" <<
					recieved_msg->ConnectingNETMYIPMask << endl;
			cout << "Sender name: " << recieved_msg->SenderName << endl;
			cout << "Receiver name: " << recieved_msg->ReceiverName << endl;
			cout << endl;
			cout << "Destination properties output format: <Subnet>/<Mask> (<Cost>):" << endl;
			cout << endl;
		}


		//Update routing table
		for (int i = 0; i < NUM_OF_ROUTERS; i++)
		{
			IF_DEBUG(ALL)
			{
				in_addr dest_temp;
				dest_temp.S_un.S_addr = recieved_msg->dest[i].DestinationNETSubnet;

				cout << "Destination " << i << " properties:" << endl;
				cout << inet_ntoa(dest_temp);
				cout << "/" << recieved_msg->dest[i].DestinationNETMask <<
												" (" << recieved_msg->dest[i].DestinationNETSubnetDistance << ")" <<
												endl;
			}

			//New dest found
			if (recieved_msg->dest[i].DestinationNETSubnet != 0)
			{
				Subnet* new_subnet = new Subnet;
				memset(new_subnet, 0, sizeof(Subnet));
				//Network order
				new_subnet->address.S_un.S_addr = (recieved_msg->dest[i].DestinationNETSubnet); //Machine orderm not network!
																					//because net2host was called when
																					//msg received
				new_subnet->mask = recieved_msg->dest[i].DestinationNETMask;
				new_subnet->cost = recieved_msg->dest[i].DestinationNETSubnetDistance;

				//Handle new message
				RoutingTable::ModifyRoute(recieved_msg->SenderName, new_subnet);
			}
		}

		//Print the new DV
		this->m_table->PrintDV();

		IF_DEBUG(TRACE)
			m_table->PrintMap();

		break;

	default:
		IF_DEBUG(ERROR)
			cout << "ERROR: Got an unhandled event." << endl;
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
	if (i >= m_num_of_routers)
	{
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
				cout << "Found neighbor: " << m_routers[i].name << endl;

			RoutingTable::AddRoute(m_my_entry.name, m_my_entry.address, 
									m_my_entry.port, *it);

			RoutingTable::AddRouter(m_routers[i].name, m_routers[i].address,
				m_routers[i].port, subnet_ptr, (*it)->cost);

			//Mark entry as neighbor
			m_routers[i].reachable = true;
		}
	}		

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

Utils::ReturnStatus MyRouter::ClearRouters()
{
	int i;
	IF_DEBUG(ALL){
		cout << "Before cleanup: " << endl;
		for (i=0; i < m_num_of_routers; i++)
			cout << m_routers[i].name << endl;
	}

	i = 0;
	while (i < m_num_of_routers)
	{
		if (!m_routers[i].reachable)
		{
			memcpy(&(m_routers[i]), &(m_routers[m_num_of_routers-1]), sizeof(RouterEntry));
			m_num_of_routers--;
			i--;
		}
		i++;
	}

	IF_DEBUG(TRACE){
		cout << "After cleanup: " << endl;
		for (i=0; i < m_num_of_routers; i++){
			cout << m_routers[i].name << ": " << inet_ntoa(m_routers[i].address) << endl;
		}
	}

	return Utils::STATUS_OK;
}
