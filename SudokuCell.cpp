#include "SudokuCell.h"

SudokuCell::SudokuCell(CellCoordinates InCoordinates)
    : Coordinates(InCoordinates)
{

}

SudokuCell::SudokuCell(CellCoordinates InCoordinates, num_t InNumber)
    : Coordinates(InCoordinates), AssignedNumber(InNumber)
{

}

void SudokuCell::AssignNumber(num_t Value)
{
    if(Value != 0 && Value <= 9)
    {
        AssignedNumber = Value;
        PossibleNumbers.clear();
    }
    else
    {
        AssignedNumber = 0;
    }
}

void SudokuCell::InitNumber(num_t Value)
{
    if (Value != 0 && Value <= 9)
    {
        AssignedNumber = Value;
        PossibleNumbers.clear();
    }
    else
    {
        AssignedNumber = 0;
    }
    bAssigningHandled = true;
}

void SudokuCell::AddPossibleNumber(num_t Value)
{
    PossibleNumbers.insert(Value);
}

void SudokuCell::RemovePossibleNumber(num_t Value)
{
    PossibleNumbers.erase(Value);
}
