#pragma once
#include <set>
#include "Miscellaneous.h"

class SudokuCell
{
public:
    SudokuCell() = default;
    SudokuCell(CellCoordinates InCoordinates);
    SudokuCell(CellCoordinates InCoordinates, num_t InNumber);

private:
    unsigned char AssignedNumber = 0;
    std::set<num_t> PossibleNumbers;
    CellCoordinates Coordinates;

public:
    void AssignNumber(num_t Value);
    void InitNumber(num_t Value);
    num_t GetAssignedNumber() const { return AssignedNumber; }
    std::set<num_t>& GetPossibleNumbers() { return PossibleNumbers; }
    void AddPossibleNumber(num_t Value);
    void RemovePossibleNumber(num_t Value);
    void SetCoordinates(CellCoordinates InCoordinates) { Coordinates = InCoordinates; }
    CellCoordinates GetCoordinates() const { return Coordinates; }
    bool bAssigningHandled = false;
};
