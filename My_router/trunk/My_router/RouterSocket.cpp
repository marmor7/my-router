#include "StdAfx.h"
#include "RouterSocket.h"

RouterSocket::RouterSocket(void)
{
}

RouterSocket::~RouterSocket(void)
{
}

//Establish a connection to a neighbour router
Utils::ReturnStatus RouterSocket::socketEstablish(){
	return Utils::STATUS_OK;
}

//Accpt a connection from a neighbour router
Utils::ReturnStatus RouterSocket::socketAccept(){
	return Utils::STATUS_OK;
}

//Receive a massage from a neighbour router
Utils::ReturnStatus RouterSocket::socketReceive(){
	return Utils::STATUS_OK;
}

//Send a massage to a neighbour router
Utils::ReturnStatus RouterSocket::socketSend(){
	return Utils::STATUS_OK;
}