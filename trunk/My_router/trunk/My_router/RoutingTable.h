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

	void RoutingTable::getDV(MyRIPMessage* msg);

	static Utils::ReturnStatus AddRoute(char name[MAX_ROUTER_NAME], in_addr* ip_array);

	static Router GetBestRoute();

private:
	static int* ms_routing_table;
};
