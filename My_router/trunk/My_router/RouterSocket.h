#pragma once
#include "Utils.h"

class RouterSocket
{
public:
	RouterSocket();
	~RouterSocket();

	//Establish a connection to a neighbor router
	static Utils::ReturnStatus SocketEstablish();

	//Accpt a connection from a neighbor router
	static Utils::ReturnStatus SocketAccept();

	//Receive a massage from a neighbor router
	static Utils::ReturnStatus SocketReceive();

	//Send a massage to a neighbor router
	static Utils::ReturnStatus SocketSend();

};
