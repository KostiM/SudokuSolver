#pragma once

#include "Miscellaneous.h"

class SudokuCell
{
public:
    SudokuCell() = default;
    SudokuCell(CellCoordinates InCoordinates);
    SudokuCell(CellCoordinates InCoordinates, num_t InNumber);

private:
    unsigned char AssignedNumber = 0;
    NumSet PossibleNumbers;
    CellCoordinates Coordinates;

public:
    void AssignNumber(num_t Value);
    num_t GetAssignedNumber() const { return AssignedNumber; }
    NumSet& GetPossibleNumbers() { return PossibleNumbers; }
    size_t GetPossibleCount();
    void AddPossibleNumber(num_t Value);
    bool KeepPossibleNumbers(NumSet BitMask);
    bool RemovePossibleNumber(num_t Value);
    bool RemovePossibleNumbers(NumSet BitMask);
    void SetCoordinates(CellCoordinates InCoordinates) { Coordinates = InCoordinates; }
    CellCoordinates GetCoordinates() const { return Coordinates; }
    bool bAssigningHandled = false;
};
