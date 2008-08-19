#include "StdAfx.h"
#include "Utils.h"


void Utils::PrintMsg(MyRIPMessage* msg)
{
	cout << "len: " << ntohs(msg->length) << "\t";
	cout << "prot: " << ntohs(msg->protocolID) << "\n";
	cout << "subnet: " << msg->ConnectingNETMYIPSubnet << "\n";
	cout << "mask: " << msg->ConnectingNETMYIPMask << "\n";
	cout << "sender name: " << msg->SenderName << "\n";
	cout << "reveiver name: " << msg->ReceiverName << "\n";
	for (int i=0; i < NUMBER_OF_DESTINATIONS; i++)
		Utils::PrintDest(i, &(msg->dest[i]));
}

void Utils::PrintDest(int i, DestinationProperties* dest)
{
	cout << "dest " << i << ": ";
	cout << "mask: " << dest->DestinationNETMask << "\t";
	cout << "subnet: " << dest->DestinationNETSubnet << "\t";
	cout << "cost: " << dest->DestinationNETSubnetDistance << endl;
}