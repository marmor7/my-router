#include "stdafx.h"
#include "RouterSocket.h"

#define AUTO_SELECT_PROTOCOL 0

int RouterSocket::ms_router_socket_sd = 0;//socket(PF_INET, SOCK_DGRAM, AUTO_SELECT_PROTOCOL);

RouterSocket::RouterSocket()
{
}

RouterSocket::~RouterSocket()
{
}

//Receive a massage from a neighbor router
Utils::SocketReturnStatus RouterSocket::SocketReceive(int& sd,
													  BYTE* buff,
													  int& len,
													  sockaddr_in *data_sender)
{
	socklen_t sender_len = sizeof(sockaddr_in);

	int bytes_recived = recvfrom(sd,								//In socket descriptor
		                         (char*) buff,						//In buffer
								 len,								//Bytes to receive
								 0,									//No flag
								 (struct sockaddr *) data_sender,	//Sender
								 &sender_len);						//Struct length

	if(bytes_recived != -1)
	{
		IF_DEBUG(ALL)
		{
			cout << "SocketReceive: Received message from socket " << sd << endl ;
			short tmp = ((MyRIPMessage*) buff)->protocolID;
			cout << "Protocol ID: "<< ntohs(tmp) << endl;
			cout << "Message received from: " << inet_ntoa(data_sender->sin_addr) << endl;
		}
	}

	len = bytes_recived;

	return (bytes_recived == -1) ? Utils::STATUS_RECIVE_FAILED : Utils::STATUS_RECIVE_OK;
}

//Send a massage to a neighbor router
Utils::SocketReturnStatus RouterSocket::SocketSend(int sd,
												   int& len,
												   BYTE* data,
												   RouterEntry& dest)
{
	if (sd <= 0)
	{
		IF_DEBUG(TRACE)
		{
			cout << "SocketSend: bad socket " << sd << endl;
		}

		return Utils::STATUS_BAD_SOCKET;
	}

	int total = 0;
	int bytesleft = len;
	int n = -1;
	sockaddr_in address;

	address.sin_family = AF_INET;				// host byte order
	address.sin_port = htons(dest.port);		// short, network byte order
	address.sin_addr = dest.address;			//Destination - network order! = inet_addr("1.2.3.4")

	memset(address.sin_zero, '\0', sizeof address.sin_zero); //Clear all other struct's fields

	while(total < len)
	{
		IF_DEBUG(TRACE)
		{
			cout << "SocketSend: Sending msg on socket " << sd << " - " ;
		}

		n = sendto(sd,								//Output socket
				   (char*) data+total,				//Output data
				   bytesleft,						//How many bytes left
				   0,								//No flags
				   (struct sockaddr *) &address,	//Remote address
				   sizeof(address));				//Size of struct

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
	return (n == -1) ? Utils::STATUS_SEND_FAILED : Utils::STATUS_SEND_OK;
}

void RouterSocket::SetConnectionParameters( struct sockaddr_in *dest, unsigned short port, char *hostname )
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

Utils::SocketReturnStatus RouterSocket::SocketClose(int& sd)
{
	//TBD: Change to close
	int result = close(sd);

	return result == 0 ? Utils::STATUS_CLOSE_OK : Utils::STATUS_CLOSE_FAILED;
}

int RouterSocket::GetRouterSocketDescriptor()
{
	return RouterSocket::ms_router_socket_sd;
}

Utils::SocketReturnStatus RouterSocket::SocketInit( unsigned short router_port )
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
	serv_addr.sin_port = htons(router_port);
	memset(serv_addr.sin_zero, '\0', sizeof serv_addr.sin_zero);

	bind_ret = bind (RouterSocket::ms_router_socket_sd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

	if(bind_ret<0)
	{
		return Utils::STATUS_BAD_BIND;
	}

	return Utils::STATUS_SOCKET_OK;
}
