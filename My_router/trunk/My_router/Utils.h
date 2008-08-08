#pragma once
#include <iostream>

using namespace std;

#define DEBUG_NONE 0
#define DEBUG_IMPORTANT 1
#define DEBUG_TRACE 2
#define DEBUG_ALL 3

#define DEBUG_LEVEL DEBUG_ALL

#define IF_DEBUG(level) if (DEBUG_##level <= DEBUG_LEVEL)

class Utils
{
public:
	Utils(void);
	~Utils(void);

	//Enumerator of the possible return statuses by InitRouter.
	enum ReturnStatus
	{
		STATUS_OK,
		STATUS_BAD_NUM_OF_ARGUMENTS,
		STATUS_BAD_FILENAME_OR_FILE_DONT_EXISTS
	};
};
