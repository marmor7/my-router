#include "stdafx.h"
#include "Utils.h"


void Utils::PrintMsg(MyRIPMessage* msg)
{
	in_addr tmp;
	tmp.s_addr = msg->ConnectingNETMYIPSubnet;
	char* subnet = inet_ntoa(tmp);

	cout << "len: " << msg->length << "\t";
	cout << "protocol: " << msg->protocolID << "\n";
	cout << "subnet: " << subnet << "\n";
	cout << "mask: " << msg->ConnectingNETMYIPMask << "\n";
	cout << "sender name: " << msg->SenderName << "\n";
	cout << "receiver name: " << msg->ReceiverName << "\n";
	for (int i=0; i < NUMBER_OF_DESTINATIONS; i++)
		if (Utils::PrintDest(i, &(msg->dest[i])) == STATUS_STOP)
			break;
}

Utils::ReturnStatus Utils::PrintDest(int i, DestinationProperties* dest)
{
	in_addr tmp;
	tmp.s_addr = dest->DestinationNETSubnet;
	char* subnet = inet_ntoa(tmp);

	cout << "dest " << i << ": ";
	cout << "subnet: " << subnet << "/";
	cout << dest->DestinationNETMask << " (";
	cout << dest->DestinationNETSubnetDistance << ") " << endl;

	if (tmp.s_addr == 0)
		return Utils::STATUS_STOP;
	else
		return Utils::STATUS_OK;
}

Utils::ReturnStatus Utils::Host2netMsg(MyRIPMessage* msg)
{
	msg->length = htons(msg->length);
	msg->protocolID = htons(msg->protocolID);
	msg->ConnectingNETMYIPSubnet = htonl(msg->ConnectingNETMYIPSubnet);
	msg->ConnectingNETMYIPMask = htonl(msg->ConnectingNETMYIPMask);

	//Sender and reciever name does not need to be converted

	//Fix all data inside destinations
	for(int i = 0; i < NUM_OF_ROUTERS; i++)
	{
		//turn mask from int rep. to bitwise rep.
		msg->dest[i].DestinationNETMask = (0xFFFFFFFF) << (32 - msg->dest[i].DestinationNETMask);
		msg->dest[i].DestinationNETMask = htonl(msg->dest[i].DestinationNETMask);

		//TMP
		//TMP

		msg->dest[i].DestinationNETSubnet = htonl(msg->dest[i].DestinationNETSubnet);
		msg->dest[i].DestinationNETSubnetDistance = htonl(msg->dest[i].DestinationNETSubnetDistance);
	}

	return STATUS_OK;
}

Utils::ReturnStatus Utils::Net2hostMsg(MyRIPMessage* msg)
{
	msg->length = ntohs(msg->length);
	msg->protocolID = ntohs(msg->protocolID);
	msg->ConnectingNETMYIPSubnet = ntohl(msg->ConnectingNETMYIPSubnet);
	msg->ConnectingNETMYIPMask = ntohl(msg->ConnectingNETMYIPMask);

	//Sender and reciever name does not need to be converted

	//Fix all data inside destinations
	for(int i = 0; i < NUM_OF_ROUTERS; i++)
	{
		msg->dest[i].DestinationNETMask = ntohl(msg->dest[i].DestinationNETMask);
		//turn mask from bitwise rep. to int rep.
		unsigned int mask = msg->dest[i].DestinationNETMask;
		unsigned int u1 = 1;
		int counter = 32;
		while ((mask & u1) == 0)
		{
			counter--;
			if (counter == 0)
				break;
			mask = mask >> 1;
		}
		msg->dest[i].DestinationNETMask = counter;
		msg->dest[i].DestinationNETSubnet = ntohl(msg->dest[i].DestinationNETSubnet);
		msg->dest[i].DestinationNETSubnetDistance = ntohl(msg->dest[i].DestinationNETSubnetDistance);
	}

	return STATUS_OK;
}
