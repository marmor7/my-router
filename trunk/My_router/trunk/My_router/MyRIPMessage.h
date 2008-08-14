#include "stdafx.h"    

#define MAX_SENDER_NAME 8 
#define MAX_RECEIVER_NAME 8
#define NUMBER_OF_DESTINATIONS 32
#define FIELD_LENGTH 4

#pragma pack(push)
#pragma pack(1)

typedef struct
{
	DWORD DestinationNETSubnet; //32 bit
	DWORD DestinationNETMask; //32 bit
	DWORD DestinationNETSubnetDistance; //32 bit
} DestinationProperties;

typedef struct
{
	BYTE length[FIELD_LENGTH];
	BYTE protocolID[FIELD_LENGTH];
	int ConnectingNETMYIPSubnet; //32 bit
	int ConnectingNETMYIPMask; //32 bit
	char SenderName[MAX_SENDER_NAME]; //64 bit
	char ReceiverName[MAX_RECEIVER_NAME]; //64 bit
	DestinationProperties dest[NUMBER_OF_DESTINATIONS];	//96 bit * NUMBER_OF_DESTINATIONS
} MyRIPMessage;

#pragma pack(pop)