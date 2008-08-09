#pragma once
#include "Utils.h"

class RoutingTable
{
public:
	RoutingTable();
	~RoutingTable();

	static void printDV();

	static Utils::ReturnStatus addRoute(char name[MAX_ROUTER_NAME], in_addr* ip_array);
};
