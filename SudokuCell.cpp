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
    if(Value>=0 && Value <= 9)
    {
        AssignedNumber = Value;
        if(Value != 0)
        {
            PossibleNumbers.reset();
        }
    }
}

size_t SudokuCell::GetPossibleCount()
{
    return PossibleNumbers.count();
}

void SudokuCell::AddPossibleNumber(num_t Value)
{
    PossibleNumbers.set(Value);
}

bool SudokuCell::KeepPossibleNumbers(NumSet BitMask)
{
    bool HasChanged = (PossibleNumbers & ~BitMask).any();
    if(HasChanged)
    {
        PossibleNumbers &= BitMask;
    }
    return HasChanged;
}

bool SudokuCell::RemovePossibleNumber(num_t Value)
{
    if (PossibleNumbers.test(Value))
    {
        PossibleNumbers.reset(Value);
        return true;
    }
    return false;
}

bool SudokuCell::RemovePossibleNumbers(NumSet BitMask)
{
    bool HasChanged = (PossibleNumbers & BitMask).any();
    if (HasChanged)
    {
        PossibleNumbers &= ~BitMask;
    }
    return HasChanged;
}
