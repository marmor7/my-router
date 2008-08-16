#pragma once
#include "stdafx.h"
#include "Utils.h"

typedef int Router; //TBD: Change to actual return type.


class RoutingTable
{
public:
	RoutingTable();

	~RoutingTable();

	void printDV();

	//Fill msg with DV data
	void GetDV(MyRIPMessage* msg);

	static Utils::ReturnStatus AddRoute(char name[MAX_ROUTER_NAME], Subnet* subnet_ptr);

	static Router GetBestRoute();

private:
	static int* ms_routing_table;

	//Sorted list for each router. First element is the best route
	static map<string, list<Subnet*> > m_routing_table;
};
