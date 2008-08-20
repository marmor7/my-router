#include "StdAfx.h"
#include "RouterSocket.h"

#define AUTO_SELECT_PROTOCOL 0

int RouterSocket::ms_router_socket_sd = 0;//socket(PF_INET, SOCK_DGRAM, AUTO_SELECT_PROTOCOL);

RouterSocket::RouterSocket()
{
}

RouterSocket::~RouterSocket()
{
}

//Establish a connection to a neighbour router
Utils::SocketReturnStatus RouterSocket::SocketEstablish(IN RouterEntry* entry)
{
	int sd, con;
	sockaddr_in router_sockaddr;

	sd = socket(PF_INET, SOCK_DGRAM, AUTO_SELECT_PROTOCOL);

	if (sd <= 0)
	{
		entry->socketId = -1;

		IF_DEBUG(ERROR)
		{
			cout << "ERROR: Establishing UDP socket to " << entry->name << 
				" on port " << entry->port << " FAILED" << endl;
		}

		return Utils::STATUS_BAD_SOCKET;
	}

	entry->socketId = sd;

	router_sockaddr.sin_family = AF_INET;
	router_sockaddr.sin_addr = entry->address;
	router_sockaddr.sin_port = htons(entry->port);
	memset(router_sockaddr.sin_zero, '\0', sizeof router_sockaddr.sin_zero);

	con = connect(sd, (struct sockaddr *) &router_sockaddr, sizeof(router_sockaddr));

	if (con != 0)
	{
		return Utils::STATUS_BAD_CONNECT;
	}

	IF_DEBUG(TRACE)
	{
		cout << "Establishing UDP socket to " << entry->name << 
				" on port " << entry->port << endl;
	}

	return Utils::STATUS_SOCKET_OK;
}

//Accept a connection from a neighbour router
Utils::SocketReturnStatus RouterSocket::SocketAccept(IN int router_socket_descriptor,
													 OUT sockaddr_in* my_router_properties, 
													 OUT int* new_sd )
{
	int sin_size;
	int* new_sd_ptr = new int;
	
	sin_size = sizeof (*my_router_properties);
	*new_sd_ptr = accept(router_socket_descriptor, (struct sockaddr *)my_router_properties, &sin_size);

	if (*new_sd_ptr == -1)
	{
		return Utils::STATUS_BAD_ACCEPT;
	}

	return Utils::STATUS_SOCKET_OK;
}

//Receive a massage from a neighbour router
Utils::SocketReturnStatus RouterSocket::SocketReceive(IN int& sd,
													  OUT BYTE* buff, 
													  IN int& len)
{
	int bytes_recived = recv(sd, (char*) buff, len, 0);

	return bytes_recived == -1 ? Utils::STATUS_RECIVE_FAILED : Utils::STATUS_RECIVE_OK;
}

//Send a massage to a neighbour router
Utils::SocketReturnStatus RouterSocket::SocketSend(IN int& socket_out, 
												   IN int& len, 
												   IN BYTE* data )
{
	//int sendto(int sockfd, const void *msg, int len, unsigned int flags, const struct sockaddr *to, socklen_t tolen); 

	if (socket_out <= 0)
	{
		IF_DEBUG(TRACE)
		{
			cout << "SocketSend: bad socket " << socket_out << endl;
		}
		
		return Utils::STATUS_BAD_SOCKET;
	}

	int total = 0;
	int bytesleft = len;
	int n = -1;
	
	while(total < len)
	{
		IF_DEBUG(TRACE)
		{
			cout << "SocketSend: Sending msg on socket " << socket_out << " - " ;
		}
		n = send(socket_out, (char*) data+total, bytesleft, 0);
		
		if (n == -1) 
		{
			IF_DEBUG(TRACE)
			{
				cout << "FAILED!" << endl;
			}
			break;
		}
		IF_DEBUG(TRACE)
		{
			cout << n << " bytes sent" << endl;
		}

		total += n;
		bytesleft -= n;
	}

	len = total;
	return n==-1 ? Utils::STATUS_SEND_FAILED : Utils::STATUS_SEND_OK;
}

void RouterSocket::SetConnectionParameters( struct sockaddr_in *dest, int port, char *hostname )
{
	struct hostent *he;
	he = gethostbyname(hostname);
	assert(he != NULL);

	memset(dest, 0, sizeof(struct sockaddr_in));
	dest->sin_family = AF_INET;
	dest->sin_port = htons(port);
	dest->sin_addr = *((struct in_addr *)he->h_addr);
	memset(dest->sin_zero, 0, sizeof(dest->sin_zero));

	IF_DEBUG(ALL)
	{
		printf("DEBUG: Server name is: %s\n", hostname);
		printf("DEBUG: Server IP is: %s\n", he->h_name);
		printf("\n");
	}
}

Utils::SocketReturnStatus RouterSocket::SocketClose( IN int& sd )
{
	//TBD: Change to close
	int result = closesocket(sd);

	return result == 0 ? Utils::STATUS_CLOSE_OK : Utils::STATUS_CLOSE_FAILED;
}

int RouterSocket::GetRouterSocketDescriptor()
{
	return RouterSocket::ms_router_socket_sd;
}

Utils::SocketReturnStatus RouterSocket::SocketInit()
{
	int bind_ret;
	struct sockaddr_in serv_addr;

	RouterSocket::ms_router_socket_sd = socket(AF_INET, SOCK_DGRAM, AUTO_SELECT_PROTOCOL);

	if (RouterSocket::ms_router_socket_sd < 0)
	{
		return Utils::STATUS_BAD_SOCKET;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(ROUTER_PORT);
	memset(serv_addr.sin_zero, '\0', sizeof serv_addr.sin_zero);

	bind_ret = bind (RouterSocket::ms_router_socket_sd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

	if(bind_ret<0)
	{
		return Utils::STATUS_BAD_BIND;
	}

	return Utils::STATUS_SOCKET_OK;
}
/*

struct sockaddr_in addr;
struct hostent* h;
h = gethostbyname(entry->name);

if (h == NULL)
{
//assert(h != NULL); //TBD: remove
return Utils::STATUS_BAD_SOCKET;
} 
entry->socketId = sd;

addr.sin_addr = entry->address;
addr.sin_port = htons(entry->port);
addr.sin_family = AF_INET;
memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

status = connect(sd, (struct sockaddr *)&addr, sizeof(struct sockaddr));


if (status == -1)
{
return Utils::STATUS_BAD_CONNECT;
}

*/