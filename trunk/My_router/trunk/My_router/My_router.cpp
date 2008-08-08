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

	my_router = NULL;

	ih = new InputHandler();
	ihrs = ih->InitRouter(argc, argv, my_router);

	switch (ihrs)
	{
	case(Utils::STATUS_OK):
		cout << "Init OK\n";
		break;
	case(Utils::STATUS_BAD_NUM_OF_ARGUMENTS):
		cout << "Init bad - wrong num of arguments\n";
		break;
	case(Utils::STATUS_BAD_FILENAME_OR_FILE_DONT_EXISTS):
		cout << "Init bad - filename or file dont exists\n";
		break;
	default:
		break;
	}

	my_router->run();

	delete (my_router);
	delete (ih);

	return 0;
} 