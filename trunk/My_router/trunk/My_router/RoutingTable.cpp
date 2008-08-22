#include "StdAfx.h"
#include "RoutingTable.h"
#include "Utils.h"

//map<const string, vector<Subnet*> > RoutingTable::m_routing_table = map<const string, vector<Subnet*> >();
vector<RoutingTableEntry>* RoutingTable::m_routing_table = new vector<RoutingTableEntry>();

struct SortRouterAddressByCost
{
	bool operator()(RouterAddress& r_a_1, RouterAddress& r_a_2)
	{
		return r_a_1.cost < r_a_2.cost;
	}
};

RoutingTable::RoutingTable()
{

}

RoutingTable::~RoutingTable()
{

}

void RoutingTable::PrintDV()
{
 	in_addr ip_addr;
 	
	cout << "My New RIP Table is:" << endl;
	cout << "To: >>>Best_Route(Best_Distance)<<< *[Alternative Routes (Alternative Distances)]" << endl;

	//Iterate over all subnets
	for (vector<RoutingTableEntry>::iterator it = RoutingTable::m_routing_table->begin();
		 it != RoutingTable::m_routing_table->end();
		 ++it)
	{
		ip_addr.S_un.S_addr = (it->first.ip_address.S_un.S_addr);
		
		//Print header:
		cout<< inet_ntoa(ip_addr) << ":" << it->first.mask << " >>>";
		
		//Print best route:
		cout << it->second->at(0).router_name << "(";
		
		//Handle INFINITY routes
		if (it->second->at(0).cost == INFINITY)
		{
			cout << "0";
		}
		else
		{
			cout << it->second->at(0).cost;
		}

		cout << ")<<< ";

		//Print additional routes
		for (vector<RouterAddress>::iterator jt = (++(it->second->begin())); //Begin at second router
			jt != it->second->end();
			++jt)
		{
			cout << jt->router_name << "(";
			
			//Handle INFINITY routes
			if (jt->cost == INFINITY)
			{
				cout << "0";
			}
			else
			{
				cout << jt->cost;
			}

			cout << ") ";
		}

		cout << endl;
	}
}


void RoutingTable::GetDV(MyRIPMessage* msg)
{
	int i = 0;
	unsigned int distance;
	//Iterate all over subnets and get minimal cost
	for (vector<RoutingTableEntry>::iterator it = RoutingTable::m_routing_table->begin();
		it != RoutingTable::m_routing_table->end();
		++it)
	{
		msg->dest[i].DestinationNETMask = htonl(it->first.mask);
		msg->dest[i].DestinationNETSubnet = htonl(it->first.ip_address.S_un.S_addr);

		distance = htonl(it->second->at(0).cost);
		
		//Convert back from INFINITY to 0
		if (distance == INFINITY)
		{
			distance = 0;
		}

		//Assign new distance
		msg->dest[i].DestinationNETSubnetDistance = distance;
		i++;
	}
}

Utils::ReturnStatus RoutingTable::AddRoute(__in char name[MAX_ROUTER_NAME],
										   __in in_addr actual_router_ip, 
										   __in unsigned short port, 
										   __in Subnet* subnet_ptr )
{
	bool add_new_vector_entry = true;
	bool router_found_in_subnet = false;
	Address new_router_subnet;
	RouterAddress new_router_addr;

	//Create new vector entry
	new_router_subnet.ip_address = subnet_ptr->address;
	new_router_subnet.mask = subnet_ptr->mask;
	
	//Create new router's list entry
	new_router_addr.cost = subnet_ptr->cost;
	new_router_addr.port = port;
	new_router_addr.router_ip = actual_router_ip;
	new_router_addr.router_name = name;

	//Check if the new router's network is already exist in our routing table
	for (vector<RoutingTableEntry>::iterator it = RoutingTable::m_routing_table->begin();
		 it != RoutingTable::m_routing_table->end();
		 ++it)
	{
		//Found!
		if (RoutingTable::CompareSubnets(new_router_subnet, it->first))
		{
			//Handle case that router's entry already exist! (update parameters)
			for (vector<RouterAddress>::iterator jt = it->second->begin();
				 jt != it->second->end();
				 ++jt)
			{
				//Router exists!
				if (strcmp(jt->router_name, name) == 0)
				{
					router_found_in_subnet = true;
					
					//Replace entry:
					it->second->erase(jt);
					it->second->push_back(new_router_addr);
					break; //jt iterator invalidates in erase(jt).
				}
			}
			
			//Router does not exist in vector, add it
			if (!router_found_in_subnet)
			{
				it->second->push_back(new_router_addr);
			}

			add_new_vector_entry = false;
			
			//Sort list to get the lowest cost first
			sort(it->second->begin(), it->second->end(), SortRouterAddressByCost());
		}
		//break; can be placed here, if an entry found then it is unique
	}

	//Subnet not found, should add new entry
	if (add_new_vector_entry)
	{
		RoutingTableEntry rte;

		//Set entry parameters
		rte.first = new_router_subnet;
		rte.second = new vector<RouterAddress>();
		rte.second->push_back(new_router_addr);

		//Add new entry to vector
		RoutingTable::m_routing_table->push_back(rte);
	}

	//TBD: Sort RoutingTable::m_routing_table ?
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