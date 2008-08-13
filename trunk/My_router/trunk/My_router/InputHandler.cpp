//TMP?
//#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "InputHandler.h"
#include "MyRouter.h"

using namespace std;
InputHandler::InputHandler() : m_all_routers(new vector<RouterEntry>)
{
	
}

InputHandler::~InputHandler()
{
	delete this->m_all_routers;
}

Utils::ReturnStatus InputHandler::InitRouter( int argc, char** argv, MyRouter** router_ptr_ptr )
{
	string line, my_ip_str, my_rip_str, start_char;
	bool my_ip_attribute, my_rip_attribute;
	ifstream in_file(FILENAME, ifstream::in);

	this->m_my_router = *router_ptr_ptr;

	my_ip_str = "[MYIP]";
	my_rip_str = "[MYRIP]";
	my_ip_attribute = false;
	my_rip_attribute = false;

	if (argc != 2)
	{
		return Utils::STATUS_BAD_NUM_OF_ARGUMENTS;
	}
	else
	{
		if (in_file == NULL)
		{
			return Utils::STATUS_BAD_FILENAME_OR_FILE_DONT_EXISTS;
		}

		this->m_my_router->SetName(argv[1]);
		IF_DEBUG(TRACE){
			cout << "My name is " << this->m_my_router->GetName() << endl;
		}

		while (getline(in_file, line))
		{
			start_char = line.at(0);
			//Line is not a comment or empty line or begin with space
			if (!(start_char == "#") && !(start_char == ""))
			{
				//Handle case of empty line
				if (line.find_first_not_of(" ") == string::npos)
				{
					continue;
				}
				
				//Check for any attributes
				if ((line.compare(0, my_ip_str.length(), my_ip_str)) == 0)
				{
					my_ip_attribute = true; 
					continue; //Next line please
				}

				if ((line.compare(0, my_rip_str.length(), my_rip_str)) == 0)
				{
					my_ip_attribute = false;
					my_rip_attribute = true;
					continue; //Next line please
				}

				if (my_ip_attribute)
				{
					this->HandleIpLine(line);
				}
				else if (my_rip_attribute)
				{
					this->HandleRipLine(line);
				}
			}
		}
		return Utils::STATUS_OK;
	}
}

Utils::ReturnStatus TextToIpPort(const string t, in_addr* ip, short* port) {
	int sum, ad1, ad2, ad3, ad4, ad5;
	sum = sscanf_s(t.c_str(), "%lu.%lu.%lu.%lu:%lu", &ad1, &ad2, &ad3, &ad4, &ad5); // Code %lu for long unsigned decimal integer
	// It sum just the IP address
	if (sum == 5) 
	{
		ip->S_un.S_un_b.s_b1 = ad1;
		ip->S_un.S_un_b.s_b2 = ad2;
		ip->S_un.S_un_b.s_b3 = ad3;
		ip->S_un.S_un_b.s_b4 = ad4;
		*port = ad5;
		return Utils::STATUS_OK;
	} 
	else
		return Utils::STATUS_BAD_IP_FORMAT;
}

void InputHandler::HandleIpLine( string line )
{
	string ip_address_and_port;
	RouterEntry router_entry;
	char router_name[MAX_ROUTER_NAME];
	int pos, last_pos;
	in_addr ip = {0};
	short port = 0;

	pos = line.find_first_not_of(" "); //Space if any at beginning
	last_pos = line.find_first_of(" ", pos);

	//Extract router's name and continue parsing other data
	memcpy(router_name, line.substr(pos, last_pos - pos).c_str(), MAX_ROUTER_NAME);

	//Find IP address and port
	line = line.substr(last_pos+1);
	pos = line.find_first_not_of(" ");
	last_pos = line.find_first_of(" ", pos);
	line = line.substr(pos, last_pos);
	ip_address_and_port = line.substr(0, last_pos - pos);

	TextToIpPort(line, &ip, &port);



	if (this->m_my_router->GetName().compare(router_name) == 0)
	{
		//If code reaches here then this is MyRouter line and
		//we need to update the routers parameters
		this->m_my_router->SetRoutersIpAndPort(ip_address_and_port.substr(0, ip_address_and_port.find_first_of(":")), port);
	}

	memset(&router_entry, 0, sizeof (struct RouterEntry));

	router_entry.address = ip;
	router_entry.port = port;
	strcpy_s((char *) &router_entry.name, 8, (char *) &router_name);
	router_entry.neighbour = 0; //Currently no neighbor.
	router_entry.socketId = 0; //Currently no open connection with router

	this->m_all_routers->push_back(router_entry);

	IF_DEBUG(TRACE)
	{
		cout << "MyIP router name: " << router_name << " "; //Do something with information
		cout << "IP & Port: " << ip_address_and_port << endl;
	}
}

void InputHandler::HandleRipLine( string line )
{
	string router_name, current_ip;
	int pos ,last_pos;
	Subnet* sub;
	bool is_my_router = false;

	pos = line.find_first_not_of(" "); //Space if any at beginning
	last_pos = line.find_first_of(" ", pos);

	//Extract router's name and continue parsing other data
	router_name = line.substr(pos, last_pos - pos);

	line = line.substr(last_pos+1);
	pos = line.find_first_not_of(" ");

	while (pos != string::npos)
	{
		line = line.substr(pos);
		pos = line.find(" ");
		
		//More spaces, continue parsing
		if (pos != string::npos) 
		{
			current_ip = line.substr(0, pos);
			line = line.substr(pos+1, line.length());
			pos = line.find_first_not_of(" ");
		}
		else
		{
			current_ip = line;
		}

		sub = this->GetSubnetStructFromString(current_ip);

		//My router
		if (router_name.compare(this->m_my_router->GetName()) == 0)
		{
			is_my_router = true;
			this->m_my_router->AddSubnet(sub);
		}
		//Other router
		else
		{
			
		}

		IF_DEBUG(TRACE)
		{
			cout << "MyRIP label: " << current_ip << endl; //Do something with information		
		}
	}

	//Not my router, update router and subnets vector
	if (!is_my_router)
	{
		
	}
}

Subnet* InputHandler::GetSubnetStructFromString( string str )
{
	string ip, mask, cost;
	Subnet* sub = new Subnet;
	memset(sub, 0, sizeof(struct Subnet));

	ip = str.substr(0, str.find_first_of(":"));
	str = str.substr(ip.length() + 1, str.length());

	mask = str.substr(0,str.find_first_of(":"));
	str = str.substr(mask.length() + 1, str.length());

	cost = str;

	sub->address.S_un.S_addr = inet_addr(ip.c_str());
	sub->cost = atoi(cost.c_str());
	sub->mask = atoi(mask.c_str());

	return sub;
}