#include "StdAfx.h"
#include "RoutingTable.h"

RoutingTable::RoutingTable(void)
{
}

RoutingTable::~RoutingTable(void)
{
}

void RoutingTable::printDV(void){
	//TBD: print DV to screen
}

void RoutingTable::GetDV(MyRIPMessage* msg){
	//TBD: fill msg with DV data
}

Utils::ReturnStatus RoutingTable::AddRoute( char name[MAX_ROUTER_NAME], in_addr* ip_array )
{
	//TBD
	return Utils::STATUS_OK;
}
