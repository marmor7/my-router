#include "stdafx.h"    

#define MAX_SENDER_NAME 8 
#define MAX_RECEIVER_NAME 8
#define NUMBER_OF_DESTINATIONS 32
#define FIELD_LENGTH 4
#define PROTOCOL_ID 1

#pragma pack(1)

typedef struct
{
	unsigned long DestinationNETSubnet; //32 bit
	int DestinationNETMask; //32 bit
	int DestinationNETSubnetDistance; //32 bit
} DestinationProperties;

typedef struct
{
	short length;
	short protocolID;
	unsigned long ConnectingNETMYIPSubnet; //32 bit
	int ConnectingNETMYIPMask; //32 bit
	char SenderName[MAX_SENDER_NAME]; //64 bit
	char ReceiverName[MAX_RECEIVER_NAME]; //64 bit
	DestinationProperties dest[NUMBER_OF_DESTINATIONS];	//96 bit * NUMBER_OF_DESTINATIONS
} MyRIPMessage;
