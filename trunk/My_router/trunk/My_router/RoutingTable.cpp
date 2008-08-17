#include "StdAfx.h"
#include "RoutingTable.h"

map<const string, vector<Subnet*> > RoutingTable::m_routing_table = map<const string, vector<Subnet*> >();

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
	string router_name_str(name);
	
	map<const string,vector<Subnet*> >::iterator iter = RoutingTable::m_routing_table.find(router_name_str);

	//Found an entry
	if (iter != RoutingTable::m_routing_table.end())
	{
		//Add subnet to list
		iter->second.push_back(subnet_ptr);

		//Sort vector by cost
		sort(iter->second.begin(), iter->second.end(), CompareSubnetsByCost());
	}
	//Router name not found in map
	else
	{
		RoutingTable::m_routing_table.insert( make_pair( router_name_str, vector<Subnet*>() ) );
		iter = RoutingTable::m_routing_table.find(router_name_str);
		
		//Find must succeed
		assert(iter != RoutingTable::m_routing_table.end());
		
		//Add new subnet to vector
		iter->second.push_back(subnet_ptr);
	}

	return Utils::STATUS_OK;
}

Router RoutingTable::GetBestRoute( in_addr address )
{
	//TBD: Change return type
	return NULL;
}