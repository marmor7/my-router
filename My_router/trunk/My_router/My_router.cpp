// My_router.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "InputHandler.h"
#include "MyRouter.h"

int main(int argc, char* argv[])
{
	MyRouter* my_router;
	InputHandler* ih;
	Utils::ReturnStatus ihrs;
	WSADATA wsaData;
	int retval;

	//Initializing winsock2
	if ((retval = WSAStartup(0x202,&wsaData)) != 0) 
	{
		fprintf(stderr,"WSAStartup failed with error %d\n",retval);
		WSACleanup();
	}

	ios::sync_with_stdio(); //C++ cout syncs with c's output libraries

	my_router = new MyRouter("");

	ih = new InputHandler();
	ihrs = ih->InitRouter(argc, argv, &my_router);

	switch (ihrs)
	{
	case(Utils::STATUS_OK):
		cout << "Init OK\n";
		my_router->Run();
		break;

	case(Utils::STATUS_BAD_NUM_OF_ARGUMENTS):
		cout << "Init bad - wrong number of arguments\n";
		break;

	case(Utils::STATUS_BAD_FILENAME_OR_FILE_DONT_EXISTS):
		cout << "Init bad - filename or file don't exists\n";
		break;

	default:
		break;
	}

	delete (my_router);
	delete (ih);

	return 0;
} 