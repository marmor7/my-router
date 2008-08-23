#include "StdAfx.h"
#include "RoutingTable.h"
#include "Utils.h"

//map<const string, vector<Subnet*> > RoutingTable::m_routing_table = map<const string, vector<Subnet*> >();
vector<RoutingTableEntry>* RoutingTable::m_routing_table = new vector<RoutingTableEntry>();
RoutersMap* RoutingTable::m_routers_map = new RoutersMap();

//Functor
struct SortRouterAddressByCost
{
	bool operator()(RouterAddress& r_a_1, RouterAddress& r_a_2)
	{
		return r_a_1.cost < r_a_2.cost;
	}
};

//Functor
struct SortRoutingTableEntry
{
	bool operator()(RoutingTableEntry& rta_1, RoutingTableEntry& rta_2)
	{
		unsigned int first_mask, second_mask, first_subnet_address, second_subnet_address;
		first_mask = 0xFFFFFFFF;
		second_mask = 0xFFFFFFFF;

		first_mask = first_mask << (32 - rta_1.first.mask);
		second_mask = second_mask << (32 - rta_2.first.mask);

		first_subnet_address = htonl(rta_1.first.ip_address.S_un.S_addr);//TBD: remove htonl
		second_subnet_address = htonl(rta_2.first.ip_address.S_un.S_addr);//TBD: remove htonl

		first_subnet_address = first_subnet_address & first_mask;
		second_subnet_address = second_subnet_address & second_mask;

		return (first_subnet_address < second_subnet_address);
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
		msg->dest[i].DestinationNETMask = it->first.mask;
		msg->dest[i].DestinationNETSubnet = it->first.ip_address.S_un.S_addr;

		distance = it->second->at(0).cost;
		
		//Convert back from INFINITY to 0
		if (distance == INFINITY)
		{
			distance = 0;
		}

		//Assign new distance
		msg->dest[i].DestinationNETSubnetDistance = distance;
		i++;
	}

	//Insures we didn't get wrong the number of spanned subnets
	assert(i < 32);
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
	//new_router_addr.port = port;TMP
	//new_router_addr.router_ip = actual_router_ip;TMP
	strncpy(new_router_addr.router_name, name, MAX_ROUTER_NAME);

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
				if (strncmp(jt->router_name, name, MAX_ROUTER_NAME) == 0)
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

	//Sort RoutingTable::m_routing_table by ascending order
	sort(RoutingTable::m_routing_table->begin(), RoutingTable::m_routing_table->end(), SortRoutingTableEntry());

	//Add entry in map:
	RoutersMap::iterator iter = RoutingTable::m_routers_map->find(string(name));
	if(iter != RoutingTable::m_routers_map->end())
	{
		RouterAddress ra;
		if(RoutingTable::GetBestRoute(subnet_ptr->address, &ra) != Utils::STATUS_OK)
		{
			IF_DEBUG(ERROR)
				cout << "Subnet not found in table!" << endl;
			return Utils::STATUS_NOT_FOUND;
		}
		if(iter->second.cost_to_router > (ra.cost + subnet_ptr->cost))
		{
			iter->second.cost_to_router = (ra.cost + subnet_ptr->cost);
			iter->second.via_subnet = *subnet_ptr;
		}
	}
	else
	{
		RouterAddress ra;
		//TBD: handle return value
		RoutingTable::GetBestRoute(subnet_ptr->address, &ra);
		RouterDetails rd;

		rd.cost_to_router = ra.cost + subnet_ptr->cost;
		rd.port = port;
		rd.router_ip = actual_router_ip;
		rd.via_subnet = *subnet_ptr;

		IF_DEBUG(TRACE)
		{
			cout << "Router entry was not found in map. Printing details: "<< endl;
			cout << "Router name: " << name << ", "<< rd.cost_to_router << ", " << rd.port
				 << ", " << inet_ntoa(rd.router_ip);
			cout << ", " << inet_ntoa(rd.via_subnet.address) << endl;
		}

		//TBD: maybe remove insert of our router?
		RoutingTable::m_routers_map->insert(make_pair(string(name), rd )); 
	}

	return Utils::STATUS_OK;
}

Utils::ReturnStatus RoutingTable::GetBestRoute( in_addr address, RouterAddress* ra )
{
	Address to_compare;
	to_compare.ip_address = address;

	vector<int>::reverse_iterator rit;
	for (vector<RoutingTableEntry>::reverse_iterator rit = RoutingTable::m_routing_table->rbegin();
		rit != RoutingTable::m_routing_table->rend();
		++rit)
	{
		to_compare.mask = rit->first.mask;
		if(RoutingTable::CompareSubnets(to_compare, rit->first))
		{
			ra->cost = rit->second->at(0).cost;
			strncpy(ra->router_name,rit->second->at(0).router_name, MAX_ROUTER_NAME);
			return Utils::STATUS_OK;
		}
	}
	return Utils::STATUS_NOT_FOUND;
}

Utils::ReturnStatus RoutingTable::GetRouterSubnet(__in RouterEntry* router, __out Subnet* subnet)
{
	RoutersMap::iterator iter = RoutingTable::m_routers_map->find(string(router->name));
	if(iter != RoutingTable::m_routers_map->end())
	{
		RouterAddress ra;
		*subnet = iter->second.via_subnet;

		return Utils::STATUS_OK;
	}
	else
	{
		return Utils::STATUS_NOT_FOUND;
	}
}

Utils::ReturnStatus RoutingTable::AddRouter(__in char name[MAX_ROUTER_NAME],__in in_addr actual_router_ip, 
		__in unsigned short port, __in Subnet* subnet_ptr, __in unsigned short cost)
{
	RouterDetails rd;
	rd.cost_to_router = cost;
	rd.port = port;
	rd.router_ip = actual_router_ip;
	rd.via_subnet = *subnet_ptr;
	rd.via_subnet.address.S_un.S_addr = ntohl(rd.via_subnet.address.S_un.S_addr);
	rd.via_subnet.address.S_un.S_addr &= ((0xFFFFFFFF) << (32 - rd.via_subnet.mask));
	rd.via_subnet.address.S_un.S_addr = htonl(rd.via_subnet.address.S_un.S_addr);

	RoutingTable::m_routers_map->insert(make_pair(string(name), rd));

	return Utils::STATUS_OK;
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

		first_subnet_address = htonl(first_address.ip_address.S_un.S_addr);//TBD: remove htonl
		second_subnet_address = htonl(second_address.ip_address.S_un.S_addr);//TBD: remove htonl

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

void RoutingTable::PrintMap()
{
	cout << "Printing map: " << endl;
	for(RoutersMap::iterator iter = RoutingTable::m_routers_map->begin();
		iter != RoutingTable::m_routers_map->end();
		++iter)
	{
		RouterDetails* rd = &(iter->second);
		Subnet temp_sub = rd->via_subnet;
		cout << iter->first << ": " << inet_ntoa(rd->router_ip) << ":" <<
			rd->port << " (" << rd->cost_to_router;
		cout << ") via: " << inet_ntoa(temp_sub.address) << "/" 
			 << rd->via_subnet.mask << endl;
	}
}