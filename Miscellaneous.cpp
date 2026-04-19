#include "Miscellaneous.h"

std::ostream& operator<<(std::ostream& os, const CellCoordinates& cc)
{
    os << '<' << cc.X << ',' << cc.Y << "> ";
    return os;
}