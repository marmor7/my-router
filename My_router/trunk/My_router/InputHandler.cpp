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

InputHandler::InputHandlerReturnStatus InputHandler::init( int argc, char** argv, MyRouter* router )
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
		return INIT_STATUS_BAD_NUM_OF_ARGUMENTS;
	}
	else
	{
		if (in_file == NULL)
		{
			return INIT_BAD_FILENAME_OR_FILE_DONT_EXISTS;
		}

		this->m_my_router = new MyRouter(argv[1]);

		while (getline(in_file, line))
		{
			start_char = line.at(0);
			//Line is not a comment or empty line or begin with space
			if (!(start_char == "#") && !(start_char == " ") && !(start_char == ""))
			{
				//cout << line << endl;
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

		return INIT_STATUS_OK;
	}
}

void InputHandler::HandleIpLine( string line )
{
	string router_name;
	int pos;

	pos = line.find(" "); //Space

	//Extract router's name and continue parsing other data
	router_name = line.substr(0, pos);
	line = line.substr(pos+1);
	pos = line.find_first_not_of(" ");
	line = line.substr(pos);
	
	while (pos != string::npos)
	{
		cout << "AAAAAAAAAAA " << line << endl; //Do something with information
		pos = line.find(" ");
		line = line.substr(pos+1);
		pos = line.find_first_not_of(" ");
		line = line.substr(pos+1);
	}
}


void InputHandler::HandleRipLine( string line )
{
	string router_name;
	int pos;

	pos = line.find(" "); //Space

	//Extract router's name and continue parsing other data
	router_name = line.substr(0, pos);
	line = line.substr(pos+1);
	pos = line.find_first_not_of(" ");
	line = line.substr(pos);

	while (pos != string::npos)
	{
		cout << "AAAAAAAAAAA " << line << endl; //Do something with information
		pos = line.find(" ");
		line = line.substr(pos+1);
		pos = line.find_first_not_of(" ");
	}
}