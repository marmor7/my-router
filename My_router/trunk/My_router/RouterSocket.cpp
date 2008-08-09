#include "StdAfx.h"
#include "RouterSocket.h"

RouterSocket::RouterSocket(void)
{
}

RouterSocket::~RouterSocket(void)
{
}

//Establish a connection to a neighbour router
Utils::ReturnStatus RouterSocket::SocketEstablish(RouterEntry* entry){
	return Utils::STATUS_OK;
}

//Accpt a connection from a neighbour router
Utils::ReturnStatus RouterSocket::SocketAccept(){
	return Utils::STATUS_OK;
}

//Receive a massage from a neighbour router
Utils::ReturnStatus RouterSocket::SocketReceive(){
	return Utils::STATUS_OK;
}

//Send a massage to a neighbour router
Utils::ReturnStatus RouterSocket::SocketSend(){
	return Utils::STATUS_OK;
}