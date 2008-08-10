#include "StdAfx.h"
#include "RouterSocket.h"

#define AUTO_SELECT_PROTOCOL 0

int RouterSocket::router_socket_sd = socket(PF_INET, SOCK_DGRAM, AUTO_SELECT_PROTOCOL);

RouterSocket::RouterSocket(void)
{
}

RouterSocket::~RouterSocket(void)
{
}

//Establish a connection to a neighbour router
Utils::SocketReturnStatus RouterSocket::SocketEstablish(IN RouterEntry* entry)
{
	int sd, status;
	sockaddr_in addr;

	sd = socket(PF_INET, SOCK_DGRAM, AUTO_SELECT_PROTOCOL);

	if (sd <= 0)
	{
		return Utils::STATUS_BAD_SOCKET;
	}

	entry->socketId = sd;

	SetConnectionParameters(&addr, entry->port, entry->name);

	status = connect(sd, (struct sockaddr *)&addr, sizeof(struct sockaddr));

	if (status == -1)
	{
		return Utils::STATUS_BAD_CONNECT;
	}

	return Utils::STATUS_SOCKET_OK;
}

//Accept a connection from a neighbour router
Utils::SocketReturnStatus RouterSocket::SocketAccept(IN int router_socket_descriptor,
													 IN sockaddr_in* my_router_properties, 
													 OUT int* new_sd)
{
	int sin_size;
	int* new_sd_ptr = new int;
	struct sockaddr_in remote_addr;
	
	sin_size = sizeof (remote_addr);
	*new_sd_ptr = accept(router_socket_descriptor, (struct sockaddr *)my_router_properties, &sin_size);

	if (*new_sd_ptr == -1)
	{
		return Utils::STATUS_BAD_ACCEPT;
	}

	return Utils::STATUS_SOCKET_OK;
}

//Receive a massage from a neighbour router
Utils::ReturnStatus RouterSocket::SocketReceive()
{
	return Utils::STATUS_OK;
}

//Send a massage to a neighbour router
Utils::SocketReturnStatus RouterSocket::SocketSend(IN int& socket_out, 
												   IN int& len, 
												   IN BYTE* data )
{
	int total = 0;
	int bytesleft = len;
	int n;
	while(total < len)
	{
		n = send(socket_out, (char*) data+total, bytesleft, 0);
		
		if (n == -1) 
		{
			break;
		}

		total += n;
		bytesleft -= n;
	}
	len = total;
	return n==-1 ? Utils::STATUS_SEND_OK : Utils::STATUS_SEND_FAILED;
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

	#ifdef _DEBUG
		printf("DEBUG: Server name is: %s\n", hostname);
		printf("DEBUG: Server IP is: %s\n", he->h_name);
		printf("\n");
	#endif
}
