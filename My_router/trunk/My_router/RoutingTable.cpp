#include "stdafx.h"
#include "RoutingTable.h"
#include "Utils.h"

vector<RoutingTableEntry>* RoutingTable::m_routing_table = new vector<RoutingTableEntry>();
RoutersMap* RoutingTable::m_routers_map = new RoutersMap();

RoutingTable::RoutingTable()
{

}

RoutingTable::~RoutingTable()
{

}

void RoutingTable::PrintDV()
{
 	in_addr ip_addr;

	cout << endl;
	cout << "My New RIP Table is:" << endl;
	cout << "To: >>>Best_Route(Best_Distance)<<< *[Alternative Routes (Alternative Distances)]" << endl;

	//Iterate over all subnets
	for (vector<RoutingTableEntry>::iterator it = RoutingTable::m_routing_table->begin();
		 it != RoutingTable::m_routing_table->end();
		 ++it)
	{
		ip_addr.s_addr = (it->first.ip_address.s_addr);
		ip_addr.s_addr = ntohl(ip_addr.s_addr);
		ip_addr.s_addr &= ((0xFFFFFFFF) << (32 - it->first.mask));
		ip_addr.s_addr = htonl(ip_addr.s_addr);

		//Print header:
		cout<< inet_ntoa(ip_addr) << ":" << it->first.mask << " >>>";

		//Print best route:
		cout << it->second->at(0).router_name << "(";

		//Handle INFINITY routes
		if (it->second->at(0).cost >= INFINITY)
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
			if (jt->cost >= INFINITY)
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
	cout << endl;
}


void RoutingTable::GetDV(MyRIPMessage* msg)
{
	int i = 0;
	int distance;
	//Iterate all over subnets and get minimal cost
	for (vector<RoutingTableEntry>::iterator it = RoutingTable::m_routing_table->begin();
		it != RoutingTable::m_routing_table->end();
		++it)
	{
		msg->dest[i].DestinationNETMask = it->first.mask;
		msg->dest[i].DestinationNETSubnet = it->first.ip_address.s_addr;

		distance = it->second->at(0).cost;

		//Convert back from INFINITY to 0
		if (distance >= INFINITY)
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

Utils::ReturnStatus RoutingTable::AddRoute(char name[MAX_ROUTER_NAME],
										   in_addr actual_router_ip,
										   unsigned short port,
										   Subnet* subnet_ptr )
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
			sort(it->second->begin(), it->second->end());
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
	sort(RoutingTable::m_routing_table->begin(), RoutingTable::m_routing_table->end());

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

		RoutingTable::GetBestRoute(subnet_ptr->address, &ra);
		RouterDetails rd;

		//Set cost to 0 - this is our router
		rd.cost_to_router = 0;
		rd.port = port;
		rd.router_ip = actual_router_ip;
		rd.via_subnet = *subnet_ptr;
		rd.alive = true;

		IF_DEBUG(TRACE)
		{
			cout << "Router entry was not found in map. Printing details: "<< endl;
			cout << "Router name: " << name << ", "<< rd.cost_to_router << ", " << rd.port
				 << ", " << inet_ntoa(rd.router_ip);
			cout << ", " << inet_ntoa(rd.via_subnet.address) << endl;
		}

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

Utils::ReturnStatus RoutingTable::GetRouterSubnet(RouterEntry* router,Subnet* subnet)
{
	RoutersMap::iterator iter = RoutingTable::m_routers_map->find(string(router->name));
	if(iter != RoutingTable::m_routers_map->end())
	{
		*subnet = iter->second.via_subnet;

		return Utils::STATUS_OK;
	}
	else
	{
		return Utils::STATUS_NOT_FOUND;
	}
}

Utils::ReturnStatus RoutingTable::AddRouter(char name[MAX_ROUTER_NAME],in_addr actual_router_ip,
											unsigned short port, Subnet* subnet_ptr, int cost )
{
	RouterDetails rd;
	rd.cost_to_router = cost;
	rd.port = port;
	rd.router_ip = actual_router_ip;
	rd.via_subnet = *subnet_ptr;
	rd.via_subnet.address.s_addr = ntohl(rd.via_subnet.address.s_addr);
	rd.via_subnet.address.s_addr &= ((0xFFFFFFFF) << (32 - rd.via_subnet.mask));
	rd.via_subnet.address.s_addr = htonl(rd.via_subnet.address.s_addr);
	rd.alive = true;

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

		first_subnet_address = ntohl(first_address.ip_address.s_addr);
		second_subnet_address = ntohl(second_address.ip_address.s_addr);

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
			<< rd->via_subnet.mask << ". alive: " << rd->alive << endl;
	}
}

Utils::ReturnStatus RoutingTable::ModifyRoute(char name[MAX_ROUTER_NAME], Subnet* subnet_ptr )
{
	//In subnet vector, find subnet_prt and check to cost from this subnet from router name
	//If our cost to subnet through router name != our cost to B + new cost then
	//Update cost in vector and update in map (if we are using the same subnet)

	Address new_router_address;
	new_router_address.ip_address = subnet_ptr->address;
	new_router_address.mask = subnet_ptr->mask;
	bool subnet_found = false;
	bool router_found_in_subnet = false;
	int cost_to_router;

	//Find cost to this neighbor
	RoutersMap::iterator iter = RoutingTable::m_routers_map->find(string(name));
	if(iter != RoutingTable::m_routers_map->end())
	{
		cost_to_router = iter->second.cost_to_router;
		//If router was dead - now alive
		if (!iter->second.alive)
		{
			iter->second.alive = true;
		}
	}
	else
	{
		return Utils::STATUS_NOT_FOUND;
	}

	for (vector<RoutingTableEntry>::iterator it = RoutingTable::m_routing_table->begin();
		 it != RoutingTable::m_routing_table->end();
		 ++it)
	{
		//Find a subnet which the new router belongs to
		if(RoutingTable::CompareSubnets(it->first, new_router_address))
		{
			subnet_found = true;

			for (vector<RouterAddress>::iterator jt = it->second->begin();
				 jt != it->second->end();
				 ++jt)
			{
				//Found the router
				if (strncmp(jt->router_name, name, MAX_ROUTER_NAME) == 0)
				{
					router_found_in_subnet = true;

					//Update the cost: cost to neighbor + cost from neighbor to dest.
					jt->cost = cost_to_router + subnet_ptr->cost;

					//Sort list
					sort(it->second->begin(), it->second->end());

					//Break from for - stop searching the routers list
					break;
				}
			}

			//Router was not found in it current subnet
			if (!router_found_in_subnet)
			{
				RouterAddress ra;
				ra.cost = subnet_ptr->cost; //TBD: Should we add our cost to subnet in addition?
				strncpy( ra.router_name, name, MAX_ROUTER_NAME);

				it->second->push_back(ra);
				router_found_in_subnet = false;

				//Sort list
				sort(it->second->begin(), it->second->end());
			}

			//break from for - stop searching the subnet vector
			break;
		}
	}

	//Subnet not found, add new subnet and the router
	if (!subnet_found)
	{
		Address addr;
		RouterAddress ra;

		vector<RouterAddress>* vec = new vector<RouterAddress>;

		addr.ip_address = subnet_ptr->address;
		addr.mask = subnet_ptr->mask;
		strncpy(ra.router_name, name, MAX_ROUTER_NAME);
		ra.cost = cost_to_router + subnet_ptr->cost;

		vec->push_back(ra);

		RoutingTable::m_routing_table->push_back(make_pair(addr, vec));

		//Sort routing table
		sort(RoutingTable::m_routing_table->begin(), RoutingTable::m_routing_table->end());
	}

	return Utils::STATUS_OK;
}

Utils::ReturnStatus RoutingTable::ReportDeadRouter(char name[MAX_ROUTER_NAME] )
{
	IF_DEBUG(TRACE)
		cout << "router " << name << " is dead" << endl;

	RoutersMap::iterator iter = RoutingTable::m_routers_map->find(string(name));
	if(iter != RoutingTable::m_routers_map->end())
	{
		Address addr;
		addr.ip_address = iter->second.via_subnet.address;
		addr.mask = iter->second.via_subnet.mask;

		//Marking router as dead
		iter->second.alive = false;

		for (vector<RoutingTableEntry>::iterator it = RoutingTable::m_routing_table->begin();
			it != RoutingTable::m_routing_table->end();
			++it)
		{
			//Find the router's name
			for (vector<RouterAddress>::iterator jt = it->second->begin();
				jt != it->second->end();
				++jt)
			{
				//Found the router
				if (strncmp(jt->router_name, name, MAX_ROUTER_NAME) == 0)
				{
					IF_DEBUG(TRACE)
						cout << "changing route to INF" << endl;

					jt->cost = INFINITY;
					//Sort table
					sort(it->second->begin(), it->second->end());
				}
			}
		}
	}
	else
	{
		return Utils::STATUS_NOT_FOUND;
	}

	return Utils::STATUS_OK;
}
