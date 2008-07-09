// My_router.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "InputHandler.h"
#include "MyRouter.h"

int main(int argc, char* argv[])
{
	MyRouter* my_router;
	InputHandler* ih;
	InputHandler::InputHandlerReturnStatus ihrs;

	my_router = NULL;

	ih = new InputHandler();
	ihrs = ih->init(argc, argv, my_router);

	switch (ihrs)
	{
	case(InputHandler::INIT_STATUS_OK):
		cout << "Init OK\n";
		break;
	default:
		break;
	}

	system("pause");

	return 0;
} 