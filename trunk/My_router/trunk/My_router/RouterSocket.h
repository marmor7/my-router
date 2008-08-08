#pragma once
#include "Utils.h"

class RouterSocket
{
public:
	RouterSocket(void);
	~RouterSocket(void);

	//Establish a connection to a neighbour router
	static Utils::ReturnStatus socketEstablish();

	//Accpt a connection from a neighbour router
	static Utils::ReturnStatus socketAccept();

	//Receive a massage from a neighbour router
	static Utils::ReturnStatus socketReceive();

	//Send a massage to a neighbour router
	static Utils::ReturnStatus socketSend();

};
