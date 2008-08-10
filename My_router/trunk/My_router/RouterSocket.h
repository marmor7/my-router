#pragma once
#include "stdafx.h"
#include "Utils.h"

class RouterSocket
{
public:
	RouterSocket();
	~RouterSocket();

	//Establish a connection to a neighbor router
	static Utils::SocketReturnStatus RouterSocket::SocketEstablish(RouterEntry* entry);

	//Accpt a connection from a neighbor router
	static Utils::SocketReturnStatus SocketAccept(IN int router_socket_descriptor,IN sockaddr_in* my_router_properties, OUT int* new_sd );

	//Receive a massage from a neighbor router
	static Utils::ReturnStatus SocketReceive();

	//Send a massage to a neighbor router
	static Utils::SocketReturnStatus SocketSend(IN int& fd, IN int& len, IN BYTE* data);

	//Close socket
	static Utils::ReturnStatus SocketClose(int sd);

private:
	static void SetConnectionParameters(struct sockaddr_in *dest, int port, char *hostname);
	static int router_socket_sd;

};
