#pragma once
#include <vector>
#include <map>
#include "SudokuCell.h"

class SudokuBoard
{
public:
    SudokuBoard();
    void Print();
    void PrintCoordinates();
    void PrintCoordinatesAndPossibleNumbers();
    void ReadContentFromFile(char* FilePath);
    void Solve();
    void PopulatePossibleNumbers(SudokuCell* InCell);
    void AssignIfSinglePossible();

    SudokuCell* GetCell(CellCoordinates InCoordinates);
    void GetRow(coord_t InRowNumber, std::vector<SudokuCell*>& OutRowVector);
    void GetColumn(coord_t InColNumber, std::vector<SudokuCell*>& OutColVector);
    void GetSquare(CellCoordinates InCoord, std::vector<SudokuCell*>& OutSquareVector);

    void GetUnsolvedInRow(coord_t InRowNumber, std::vector<SudokuCell*>& OutRowVector);
    void GetUnsolvedInColumn(coord_t InColNumber, std::vector<SudokuCell*>& OutColVector);
    void GetUnsolvedInSquare(CellCoordinates InCoord, std::vector<SudokuCell*>& OutSquareVector);
    void GetUnsolvedMapFromArray(std::vector<SudokuCell*>& InVector, std::map<num_t, std::vector<SudokuCell*>>& OutMap);

    bool IsSolved();
    void SolveByRows();
    void SolveByCols();
    void SolveBySquares();
    void SolveArray(std::vector<SudokuCell*>& InVector);
    void FindObviousTuples();
    void FindObviousTuplesInArray(std::vector<SudokuCell*>& InVector);

    void AssingNumberAndHandle(SudokuCell * const  Cell, const num_t Number);
    void HandleCellAssigning(SudokuCell* Cell);

private:
    std::vector<SudokuCell> Cells{81,SudokuCell{{0,0}}};
    num_t UnsolvedCellsNumber = 0;
    bool bChangedOnPreviousCycle = false;
};

