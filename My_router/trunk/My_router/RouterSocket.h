#pragma once
#include "stdafx.h"
#include "Utils.h"

class RouterSocket
{
public:
	//************************************
	// Method:    RouterSocket
	// FullName:  RouterSocket::RouterSocket
	// Access:    public
	// Returns:
	// Qualifier: Constructor
	//************************************
	RouterSocket();

	//************************************
	// Method:    ~RouterSocket
	// FullName:  RouterSocket::~RouterSocket
	// Access:    public
	// Returns:
	// Qualifier: Destructor
	//************************************
	~RouterSocket();

	//************************************
	// Method:    SocketReceive
	// FullName:  RouterSocket::SocketReceive
	// Access:    public
	// Returns:   Utils::SocketReturnStatus
	// Qualifier: Receive a massage from a neighbor router
	// Parameter: IN int & sd - Remote router's connection socket
	// Parameter: OUT BYTE * buff - Pointer to buffer that will be filled
	// Parameter: IN int & len - Length of data to receive
	//************************************
	static Utils::SocketReturnStatus SocketReceive(int& sd, BYTE* buff, int& len, sockaddr_in *data_sender);

	//************************************
	// Method:    SocketSend
	// FullName:  RouterSocket::SocketSend
	// Access:    public
	// Returns:   Utils::SocketReturnStatus
	// Qualifier: Send a massage to a neighbor router
	// Parameter: IN int sd - Remote router's connection socket
	// Parameter: IN int & len - Number of bytes to send
	// Parameter: IN BYTE * data - Pointer to data
	// Parameter: IN RouterEntry & dest - Remote computer destination
	//************************************
	static Utils::SocketReturnStatus SocketSend(int sd, int& len, BYTE* data, RouterEntry& dest);

	//************************************
	// Method:    SocketClose
	// FullName:  RouterSocket::SocketClose
	// Access:    public
	// Returns:   Utils::SocketReturnStatus
	// Qualifier: Close socket
	// Parameter: IN int & sd - Socket descriptor
	//************************************
	static Utils::SocketReturnStatus SocketClose(int& sd);

	//************************************
	// Method:    SocketInit
	// FullName:  RouterSocket::SocketInit
	// Access:    public static
	// Returns:   Utils::SocketReturnStatus
	// Qualifier: Initializes the router's socket
	//************************************
	static Utils::SocketReturnStatus SocketInit(unsigned short router_port);

	//************************************
	// Method:    GetRouterSocketDescriptor
	// FullName:  RouterSocket::GetRouterSocketDescriptor
	// Access:    public static
	// Returns:   int
	// Qualifier: Gets the router sd used for listen
	//************************************
	static int GetRouterSocketDescriptor();

private:
	//************************************
	// Method:    SetConnectionParameters
	// FullName:  RouterSocket::SetConnectionParameters
	// Access:    private static
	// Returns:   void
	// Qualifier: Sets up the connection parameters to use by SocketEstablish()
	// Parameter: struct sockaddr_in * dest
	// Parameter: unsigned short port
	// Parameter: char * hostname
	//************************************
	static void SetConnectionParameters(struct sockaddr_in *dest, unsigned short port, char *hostname);

	//Router's socket descriptor
	static int ms_router_socket_sd;

};
