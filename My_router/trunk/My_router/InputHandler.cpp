#include "stdafx.h"
#include "InputHandler.h"
#include "MyRouter.h"

using namespace std;

InputHandler::InputHandler()
{
	
}

InputHandler::~InputHandler()
{

}

Utils::ReturnStatus InputHandler::InitRouter( int argc, char** argv, MyRouter* router )
{
	string line, my_ip_str, my_rip_str, start_char;
	bool my_ip_attribute, my_rip_attribute;
	ifstream in_file(FILENAME, ifstream::in);

	this->m_my_router = router;

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

		this->m_my_router = new MyRouter(argv[1]);
		cout << "My name is " << this->m_my_router->GetName() << endl;

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

void InputHandler::HandleIpLine( string line )
{
	string router_name, ip_address_and_port;
	int pos, last_pos;

	pos = line.find_first_not_of(" "); //Space if any at beginning
	last_pos = line.find_first_of(" ", pos);

	//Extract router's name and continue parsing other data
	router_name = line.substr(pos, last_pos - pos);

	//Find IP address and port
	line = line.substr(last_pos+1);
	pos = line.find_first_not_of(" ");
	last_pos = line.find_first_of(" ", pos);
	ip_address_and_port = line.substr(pos, last_pos - pos);

	cout << "MyIP router name: " << router_name << " "; //Do something with information
	cout << "IP & Port: " << ip_address_and_port << endl;
}

void InputHandler::HandleRipLine( string line )
{
	string router_name, current_ip;
	int pos ,last_pos;

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
		
		cout << "MyRIP label: " << current_ip << endl; //Do something with information		
	}
}