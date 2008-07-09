#include "stdafx.h"
#include "MyRouter.h"

MyRouter::MyRouter()
{
	this->my_name = "";
}

MyRouter::MyRouter( string name )
{
	this->my_name = name;
}

std::string MyRouter::GetName()
{
	return this->my_name;
}

void MyRouter::SetName( string name )
{
	this->my_name = name;
}