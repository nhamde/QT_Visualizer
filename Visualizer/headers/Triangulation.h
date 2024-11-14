#pragma once
#include "Triangle.h"
#include<vector>

class Triangulation
{
public:
    Triangulation();
    ~Triangulation();

    std::vector<double> UniqueNumbers;
    std::vector<Triangle> Triangles;
};
