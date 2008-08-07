#pragma once
#include "stdafx.h"

//Fwd deceleration
class MyRouter;

#define NUMBER_OF_ALLOWED_ARGUMENTS 2
#define MAX_FILENAME_LENGTH 256
#define FILENAME "myrouter.cfg"

using namespace std;

class InputHandler
{
public:
	//************************************
	// Method:    InputHandler
	// FullName:  InputHandler::InputHandler
	// Access:    public 
	// Returns:   
	// Qualifier: Constructor
	//************************************
	InputHandler();

	
	//************************************
	// Method:    ~InputHandler
	// FullName:  InputHandler::~InputHandler
	// Access:    public 
	// Returns:   
	// Qualifier: Destructor
	//************************************
	~InputHandler();

	//Enumerator of the possible return statuses by InitRouter.
	enum InputHandlerReturnStatus
	{
		INIT_STATUS_OK,
		INIT_STATUS_BAD_NUM_OF_ARGUMENTS,
		INIT_BAD_FILENAME_OR_FILE_DONT_EXISTS
	};
	
	//************************************
	// Method:    InitRouter
	// FullName:  InputHandler::InitRouter
	// Access:    public 
	// Returns:   init_status - Status of initiation
	// Qualifier:
	// Parameter: int argc - Number of arguments from user
	// Parameter: char** argv - Argument count
	//************************************
	InputHandlerReturnStatus InitRouter(int argc, char** argv, MyRouter* router);

protected:
	//Filename of input file
	string filename;

	//Program arguments
	int myArgc; char** myArgv;

	MyRouter* m_my_router;

	//************************************
	// Method:    HandleIpLine
	// FullName:  InputHandler::HandleIpLine
	// Access:    protected 
	// Returns:   void
	// Qualifier: Handle a line that is identified with [MYIP] attribute
	// Parameter: string line
	//************************************
	void HandleIpLine(string line);

	//************************************
	// Method:    HandleRipLine
	// FullName:  InputHandler::HandleRipLine
	// Access:    protected 
	// Returns:   void
	// Qualifier: Handle a line that is identified with [MYRIP] attribute
	// Parameter: string line
	//************************************
	void HandleRipLine(string line);

private:
};