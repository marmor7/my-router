#include "StdAfx.h"
#include "Utils.h"


void Utils::PrintMsg(MyRIPMessage* msg)
{
	in_addr tmp;
	tmp.S_un.S_addr = ntohl(msg->ConnectingNETMYIPSubnet);
	char* subnet = inet_ntoa(tmp);
	
	cout << "len: " << ntohs(msg->length) << "\t";
	cout << "protocol: " << ntohs(msg->protocolID) << "\n";
	cout << "subnet: " << subnet << "\n";
	cout << "mask: " << ntohl(msg->ConnectingNETMYIPMask) << "\n";
	cout << "sender name: " << msg->SenderName << "\n";
	cout << "receiver name: " << msg->ReceiverName << "\n";
	for (int i=0; i < NUMBER_OF_DESTINATIONS; i++)
		Utils::PrintDest(i, &(msg->dest[i]));
}

void Utils::PrintDest(int i, DestinationProperties* dest)
{
	in_addr tmp;
	tmp.S_un.S_addr = ntohl(dest->DestinationNETSubnet);
	char* subnet = inet_ntoa(tmp);

	cout << "dest " << i << ": ";
	cout << "mask: " << ntohl(dest->DestinationNETMask) << "\t";
	cout << "subnet: " << subnet << "\t";
	cout << "cost: " << ntohl(dest->DestinationNETSubnetDistance) << endl;
}