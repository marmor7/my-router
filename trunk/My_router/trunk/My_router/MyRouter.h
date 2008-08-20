#pragma once;
#include "stdafx.h"
#include "Utils.h"

//Fwd declarations
class EventHandler;
class RoutingTable;

using namespace std;

class MyRouter
{
public:

	//************************************
	// Method:    ~MyRouter
	// FullName:  MyRouter::~MyRouter
	// Access:    public 
	// Returns:   
	// Qualifier: Destructor
	//************************************
	~MyRouter();

	//************************************
	// Method:    MyRouter
	// FullName:  MyRouter::MyRouter
	// Access:    public 
	// Returns:   
	// Qualifier: Constructor
	// Parameter: string name - Router's name
	//************************************
	MyRouter(string name);

	//************************************
	// Method:    GetName
	// FullName:  MyRouter::GetName
	// Access:    public 
	// Returns:   std::string
	// Qualifier: Gets router's name
	//************************************
	string GetName();

	//************************************
	// Method:    SetName
	// FullName:  MyRouter::SetName
	// Access:    public 
	// Returns:   void
	// Qualifier: Sets a new name to the router
	// Parameter: string name
	//************************************
	void SetName(string name);

	//************************************
	// Method:    AddRouter
	// FullName:  MyRouter::AddRouter
	// Access:    public 
	// Returns:   Utils::ReturnStatus
	// Qualifier: Add a router (name, ip, port)
	// Parameter: char name[8]
	// Parameter: in_addr * address
	// Parameter: short port
	//************************************
	Utils::ReturnStatus AddRouter(char name[MAX_ROUTER_NAME], in_addr* address, short port);

	//************************************
	// Method:    AddRoute
	// FullName:  MyRouter::AddRoute
	// Access:    public 
	// Returns:   Utils::ReturnStatus
	// Qualifier: Adds a subnets array for a router
	// Parameter: char name[MAX_ROUTER_NAME]
	// Parameter: in_addr * ip_array
	// Parameter: int mask
	//************************************
	Utils::ReturnStatus AddRoute(char name[MAX_ROUTER_NAME], Subnet* subnet_ptr);

	//************************************
	// Method:    Run
	// FullName:  MyRouter::Run
	// Access:    public 
	// Returns:   void
	// Qualifier: Main loop of the router
	//************************************
	void Run();

	//************************************
	// Method:    SetRoutersIpAndPort
	// FullName:  MyRouter::SetRoutersIpAndPort
	// Access:    public 
	// Returns:   void
	// Qualifier: Set the routers internal configuration of its own IP and port
	// Parameter: string ip
	// Parameter: short port
	//************************************
	void SetRoutersIpAndPort(string ip, short port);

	//************************************
	// Method:    GetMySubnets
	// FullName:  MyRouter::GetMySubnets
	// Access:    public 
	// Returns:   vector<Subnet*>
	// Qualifier: Gets the vector containing the router's subnets which it is connected to
	//************************************
	vector<Subnet*> GetMySubnets();

	//************************************
	// Method:    AddSubnet
	// FullName:  MyRouter::AddSubnet
	// Access:    public 
	// Returns:   void
	// Qualifier: Adds a subnet to subnet's vector
	// Parameter: Subnet * subnet
	//************************************
	void AddSubnet(Subnet* subnet);

protected:
	//Initialize the fd_sets
	void initSets();

	//Display an fd_set
	void displaySet(string title, fd_set & set);

	//Router's name
	string m_name;

	sockaddr_in m_router_ip; //Remove
 
	short m_router_port; //Remove

	//Incoming event handler
	EventHandler* m_handler;

	//Router's routing table
	RoutingTable* m_table;

	//Router database (name, ip, port)
	RouterEntry* m_routers;
	RouterEntry* m_my_entry;

	//Number of neighboring routers
	int m_num_of_routers;

	//List of router's subnets
	vector<Subnet*>* m_my_router_subnets;

	//Sets of sockets - used by select function
	fd_set m_active_fd_set, m_read_fd_set, m_write_fd_set;

	//Array from index to socketId
	int m_sockets[NUM_OF_ROUTERS];

	//Max socket descriptor
	int m_max_fd;

	//This router socket descriptor
	int m_my_fd;

private:
};