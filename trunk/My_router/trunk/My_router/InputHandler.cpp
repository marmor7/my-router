//TMP?
//#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "InputHandler.h"
#include "MyRouter.h"

using namespace std;
InputHandler::InputHandler() : m_all_routers(new vector<RouterEntry>), m_idx(0)
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
	char router_name_c_srt[MAX_ROUTER_NAME];
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
			//skip empty lines
			if (line.length() <= 0)
				continue;

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

		for (int i = 0; i < this->m_idx; i++)
		{
			strcpy_s(router_name_c_srt, MAX_ROUTER_NAME, this->m_rip_name[i].c_str());
			this->m_my_router->AddRoute(router_name_c_srt, this->m_rip_subnet[i]);
		}
		return Utils::STATUS_OK;
	}
}

Utils::ReturnStatus TextToIpPort(const string t, in_addr* ip, unsigned short* port) {
	int sum, ad1, ad2, ad3, ad4, ad5;
	sum = sscanf_s(t.c_str(), "%lu.%lu.%lu.%lu:%lu", &ad1, &ad2, &ad3, &ad4, &ad5); // Code %lu for long unsigned decimal integer
	// It sum just the IP address
	if (sum == 5) 
	{
		ip->S_un.S_un_b.s_b1 = ad1;
		ip->S_un.S_un_b.s_b2 = ad2;
		ip->S_un.S_un_b.s_b3 = ad3;
		ip->S_un.S_un_b.s_b4 = ad4;
		*port = (unsigned short int) ad5;
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
	unsigned short int port = 0;

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
	strcpy_s((char *) &router_entry.name, MAX_ROUTER_NAME, (char *) &router_name);
	router_entry.neighbour = false; //Currently no neighbor.
	//router_entry.socketId = 0; //Currently no open connection with router

	this->m_all_routers->push_back(router_entry);

	this->m_my_router->AddRouter(router_name, &ip, port);

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
	Subnet* sub_ptr;
	vector<Subnet*>* subnets_vector_ptr = new vector<Subnet*>();
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

		sub_ptr = this->GetSubnetStructFromString(current_ip);

		//My router
		if (router_name.compare(this->m_my_router->GetName()) == 0)
		{
			is_my_router = true; //TBD: Can be removed if won't be used later
			this->m_my_router->AddSubnet(sub_ptr);
		}
		//Other router
		else
		{
			subnets_vector_ptr->push_back(sub_ptr);
		}

		IF_DEBUG(ALL)
		{
			cout << "MyRIP label: " << current_ip << endl;	
		}
	}
	
	if (subnets_vector_ptr->size() > 0)
	{
		for (vector<Subnet*>::iterator it = subnets_vector_ptr->begin();
			it != subnets_vector_ptr->end();
			++it)
		{
			this->m_rip_name[this->m_idx] = router_name;
			this->m_rip_subnet[this->m_idx] = *it;
			this->m_idx++;
		}
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