#include "StdAfx.h"
#include "RoutingTable.h"

//map<const string, vector<Subnet*> > RoutingTable::m_routing_table = map<const string, vector<Subnet*> >();
vector<RoutingTableEntry>* RoutingTable::m_routing_table = new vector<RoutingTableEntry>();

RoutingTable::RoutingTable()
{

}

RoutingTable::~RoutingTable()
{

}

void RoutingTable::PrintDV(){
	//TBD: print DV to screen
}

void RoutingTable::GetDV(MyRIPMessage* msg){
	//TBD: fill msg with DV data
}

Utils::ReturnStatus RoutingTable::AddRoute(char name[MAX_ROUTER_NAME], Subnet* subnet_ptr)
{
	//Continue here
	return Utils::STATUS_OK;
}

in_addr RoutingTable::GetBestRoute( in_addr address )
{
	in_addr ret;

	return ret;
}