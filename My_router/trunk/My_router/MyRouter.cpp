#include "stdafx.h"
#include "MyRouter.h"

MyRouter::MyRouter() : m_name("")
{

}

MyRouter::~MyRouter()
{

}

MyRouter::MyRouter( string name )
{
	this->m_name = name;
}

std::string MyRouter::GetName()
{
	return this->m_name;
}

void MyRouter::SetName( string name )
{
	this->m_name = name;
}

void MyRouter::run()
{
	EventHandler* handler = new EventHandler();

	handler->handle(EventHandler::RT_EVENT_READ_CONFIG);

	while (true)
	{
		break;
	}

	delete(handler);
}