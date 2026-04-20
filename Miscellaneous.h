#pragma once
#include <iostream>
#include <bitset>

typedef unsigned int num_t;
typedef unsigned int coord_t;
using NumSet = std::bitset<10>;
constexpr coord_t INVALID_COORD = -1;

struct CellCoordinates
{
    CellCoordinates(coord_t inX, coord_t inY)
    :X(inX),Y(inY)
    {}

    coord_t X = 0;
    coord_t Y = 0;

    friend std::ostream& operator<<(std::ostream& os, const CellCoordinates& cc);
};
