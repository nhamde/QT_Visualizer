#pragma once
#include<string>
#include "Triangulation.h"

class Reader
{
	virtual void read(const std::string& fileName, Triangulation& triangulation) = 0;
};
