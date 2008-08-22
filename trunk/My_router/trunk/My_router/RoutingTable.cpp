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

Utils::ReturnStatus RoutingTable::AddRoute(__in char name[MAX_ROUTER_NAME],
										   __in in_addr actual_router_ip, 
										   __in unsigned short port, 
										   __in Subnet* subnet_ptr )
{
	bool add_new_vector_entry = true;
	Address new_router_addr;

	new_router_addr.ip_address = subnet_ptr->address;
	new_router_addr.mask = subnet_ptr->mask;

	//Check if the new router's network is already exist in our routing table
	for (vector<RoutingTableEntry>::iterator it = RoutingTable::m_routing_table->begin();
		 it != RoutingTable::m_routing_table->end();
		 ++it)
	{
		//Found!
		if (RoutingTable::CompareSubnets(new_router_addr, it->first))
		{
			add_new_vector_entry = false;
		}
	}

	//Subnet not found, should add new entry
	if (add_new_vector_entry)
	{

	}
	return Utils::STATUS_OK;
}

RouterAddress RoutingTable::GetBestRoute( in_addr address )
{
	RouterAddress ret = {0};

	return ret;
}

bool RoutingTable::CompareSubnets( Address first_address, Address second_address )
{
	unsigned int mask, first_subnet_address, second_subnet_address;
	mask = 0xFFFFFFFF;
	IF_DEBUG(ALL)
	{
		cout << "RoutingTable::Comparing 2 subnets:" << endl;
		cout << "First subnet: " << inet_ntoa(first_address.ip_address) << 
			" with mask " << first_address.mask << endl;
		cout << "Second subnet: " << inet_ntoa(second_address.ip_address) << 
			" with mask " << second_address.mask << endl;
	}

	//If the 2 masks are different always return false
	if (first_address.mask != second_address.mask)
	{
		IF_DEBUG(ALL)
		{
			cout << "Mask are not equal, returning false." << endl;
		}

		return false;
	}
	else
	{
		mask = mask << (32 - first_address.mask);

		first_subnet_address = htonl(first_address.ip_address.S_un.S_addr);
		second_subnet_address = htonl(second_address.ip_address.S_un.S_addr);

		first_subnet_address = first_subnet_address & mask;
		second_subnet_address = second_subnet_address & mask;

		if (first_subnet_address == second_subnet_address)
		{
			IF_DEBUG(ALL)
			{
				cout << "Subnets equals, returning true." << endl;
			}

			return true;
		}
		else
		{
			IF_DEBUG(ALL)
			{
				cout << "Subnets are not equal, returning false." << endl;
			}
			return false;
		}
	}
}