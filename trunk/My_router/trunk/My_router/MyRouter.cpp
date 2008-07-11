#include "stdafx.h"
#include "MyRouter.h"

MyRouter::MyRouter()
{
	this->m_name = "";
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