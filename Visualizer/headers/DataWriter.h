#pragma once
#include "Writer.h"
#include "Triangulation.h"

class DataWriter : public Writer
{
public:
    void Write(const std::string& filename, const Triangulation& triangulation);

private:
    std::string fomulateText(const Triangulation& triangulation, const Point& point); // edited to use const reference
};
