#include "SudokuBoard.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>

SudokuBoard::SudokuBoard()
{
    for(int i = 0; i <81; i++)
    {
        Cells[i].SetCoordinates({ coord_t (i%9), coord_t(i/9) });
    }
}

void SudokuBoard::ReadContentFromFile(char* FilePath)
{
    std::ifstream File(FilePath);

    if(!File.is_open())
    {
        std::cout << "File " << FilePath << "could'n be open.";
        return;
    }
    std::string Entry;
    size_t CellSize = Cells.size();
    for(int i = 0; !File.eof(); ++i)
    {
        if (i > CellSize)
        {
            break;
        }
        File >> Entry;

        if(i < Cells.size() -1)
        {
            Cells[i].AssignNumber(std::stoi(Entry));
        }
    }
}

void SudokuBoard::Solve()
{
    for(auto& Cell : Cells)
    {
        if(Cell.GetAssignedNumber() == 0)
        {
            ++UnsolvedCellsNumber;
            PopulatePossibleNumbers(&Cell);
        }
    }
    
    while(!IsSolved())
    {
        bChangedOnPreviousCycle = false;
        AssignIfSinglePossible();
        SolveByRows();
        SolveByCols();
        SolveBySquares();

        if(!bChangedOnPreviousCycle)
        {
            FindObviousTuples();
        }

        if (!bChangedOnPreviousCycle)
        {
            std::cout << "\nIt's stuck, can't be solved entirely, that's it for now: \n";
            Print();
            break;
        }
    }

    if(IsSolved())
    {
        std::cout << "\nIt's done, here's the solution:\n";
        Print();
    }
    else
    {
        PrintCoordinatesAndPossibleNumbers();
    }
}

void SudokuBoard::Print()
{
    for (auto Cell : Cells)
    {
        if(Cell.GetAssignedNumber() == 0)
            std::cout << "  ";
        else
            std::cout << Cell.GetAssignedNumber() << ' ';
        if (Cell.GetCoordinates().X == 2 || Cell.GetCoordinates().X == 5)
        {
            std::cout << "| ";
        }
        if (Cell.GetCoordinates().X == 8)
        {
            std::cout << std::endl;
            if (Cell.GetCoordinates().Y == 2 || Cell.GetCoordinates().Y == 5)
            {
                std::cout << "---------------------" << std::endl;
            }
        }
    }
}

void SudokuBoard::PrintCoordinates()
{
    for(auto Cell : Cells)
    {
        auto Coord = Cell.GetCoordinates();
        std::cout << Coord;
        if(Coord.X == 8)
        {
            std::cout << std::endl;
        }
    }
}

void SudokuBoard::PrintCoordinatesAndPossibleNumbers()
{
    std::cout << std::endl;
    for (auto& Cell : Cells)
    {
        if (Cell.GetAssignedNumber() == 0 )
        {
            auto Coord = Cell.GetCoordinates();
            std::cout << '<' << Coord.X << ',' << Coord.Y << "> : ";
            for( auto num :Cell.GetPossibleNumbers())
            {
                std::cout << num << ' ';
            }
            std::cout << std::endl;
        }
    }
}

void SudokuBoard::PopulatePossibleNumbers(SudokuCell* InCell)
{    
    bool CheckMarks[9];
    for (bool& Check : CheckMarks)
    {
        Check = false;
    }

    auto Coordinates = InCell->GetCoordinates();
    std::vector<SudokuCell*> DependencyCells;

    GetRow(Coordinates.Y, DependencyCells);
    for (auto Cell : DependencyCells)
    {
        if (Cell->GetAssignedNumber() != 0)
        {
            CheckMarks[Cell->GetAssignedNumber() - 1] = true;
        }
    }

    GetColumn(Coordinates.X, DependencyCells);
    for (auto Cell : DependencyCells)
    {
        if (Cell->GetAssignedNumber() != 0)
        {
            CheckMarks[Cell->GetAssignedNumber() - 1] = true;
        }
    }
    
    GetSquare(Coordinates, DependencyCells);
    for (auto Cell : DependencyCells)
    {
        if (Cell->GetAssignedNumber() != 0)
        {
            CheckMarks[Cell->GetAssignedNumber() - 1] = true;
        }
    }

    for (int i = 0; i < 9; ++i)
    {
        if (CheckMarks[i] == false)
        {
            InCell->AddPossibleNumber(i+1);
        }
    }
}

void SudokuBoard::AssignIfSinglePossible()
{
    for(auto& Cell : Cells)
    {
        if(Cell.GetPossibleNumbers().size() == 1)
        {
            AssingNumberAndHandle(&Cell, *Cell.GetPossibleNumbers().begin());
        }
    }
}

SudokuCell* SudokuBoard::GetCell(CellCoordinates InCoordinates)
{
    size_t idx = InCoordinates.X + 9 * InCoordinates.Y;
    return &Cells[idx];
}

void SudokuBoard::GetRow(coord_t InRowNumber, std::vector<SudokuCell*>& OutRowVector)
{
    OutRowVector.clear();
    OutRowVector.reserve(9);
    for(coord_t i = 0; i < 9; i++)
    {
        OutRowVector.emplace_back( GetCell({i,InRowNumber}) );
    }
}

void SudokuBoard::GetColumn(coord_t InColNumber, std::vector<SudokuCell*>& OutColVector)
{
    OutColVector.clear();
    OutColVector.reserve(9);
    for (coord_t i = 0; i < 9; i++)
    {
        OutColVector.emplace_back(GetCell({InColNumber,i}));
    }
}

void GetOffsets(coord_t InCoord, int OutOffsets[3])
{
    switch(InCoord%3)
    {
    case 0:
        OutOffsets[0] = 0;
        OutOffsets[1] = 1;
        OutOffsets[2] = 2;
        break;
    case 1:
        OutOffsets[0] = -1;
        OutOffsets[1] = 0;
        OutOffsets[2] = 1;
        break;
    case 2:
        OutOffsets[0] = -2;
        OutOffsets[1] = -1;
        OutOffsets[2] = 0;
        break;
    } 
}

void SudokuBoard::GetSquare(CellCoordinates InCoord, std::vector<SudokuCell*>& OutSquareVector)
{
    OutSquareVector.clear();
    OutSquareVector.reserve(9);

    int XOffsets[3];
    GetOffsets(InCoord.X,XOffsets);
    int YOffsets[3];
    GetOffsets(InCoord.Y, YOffsets);

    for (int YOffset : YOffsets)
    {
        for(int XOffset: XOffsets)
        {
            OutSquareVector.push_back(GetCell({InCoord.X + XOffset, InCoord.Y + YOffset}));
        }
    }
}

void SudokuBoard::GetUnsolvedInRow(coord_t InRowNumber, std::vector<SudokuCell*>& OutRowVector)
{
    OutRowVector.clear();
    OutRowVector.reserve(9);
    SudokuCell* Cellptr = nullptr;
    for (coord_t i = 0; i < 9; i++)
    {
        Cellptr = GetCell({ i,InRowNumber });
        if (Cellptr->GetAssignedNumber() == 0)
        {
            OutRowVector.push_back(Cellptr);
        }
    }
}

void SudokuBoard::GetUnsolvedInColumn(coord_t InColNumber, std::vector<SudokuCell*>& OutColVector)
{
    OutColVector.clear();
    OutColVector.reserve(9);
    SudokuCell* Cellptr = nullptr;
    for (coord_t i = 0; i < 9; i++)
    {
        Cellptr = GetCell({InColNumber, i});
        if (Cellptr->GetAssignedNumber() == 0)
        {
            OutColVector.push_back(Cellptr);
        }
    }
}

void SudokuBoard::GetUnsolvedInSquare(CellCoordinates InCoord, std::vector<SudokuCell*>& OutSquareVector)
{
    OutSquareVector.clear();
    OutSquareVector.reserve(9);

    int XOffsets[3];
    GetOffsets(InCoord.X, XOffsets);
    int YOffsets[3];
    GetOffsets(InCoord.Y, YOffsets);

    SudokuCell* Cellptr = nullptr;
    for (int YOffset : YOffsets)
    {
        for (int XOffset : XOffsets)
        {
            Cellptr = GetCell({ InCoord.X + XOffset, InCoord.Y + YOffset });
            if (Cellptr->GetAssignedNumber() == 0)
            {
                OutSquareVector.push_back(Cellptr);
            }
        }
    }
}

void SudokuBoard::GetUnsolvedMapFromArray(std::vector<SudokuCell*>& InVector, std::map<num_t, std::vector<SudokuCell*>>& OutMap)
{
    OutMap.clear();
    for (auto Cell : InVector)
    {
        for (auto& PossibleNumber : Cell->GetPossibleNumbers())
        {
            auto Pair = OutMap.find(PossibleNumber);
            if (Pair != OutMap.end())
            {
                Pair->second.push_back(Cell);
            }
            else
            {
                OutMap.insert({ PossibleNumber , {Cell} });
            }
        }
    }
}

bool SudokuBoard::IsSolved()
{
    return !(UnsolvedCellsNumber > 0);
}

void SudokuBoard::SolveByRows()
{
    for(int i = 0; i < 8; ++i)
    {
        std::vector<SudokuCell*> Row;
        GetUnsolvedInRow(i,Row);
        SolveArray(Row);
    } 
}

void SudokuBoard::SolveByCols()
{
    for (int i = 0; i < 8; ++i)
    {
        std::vector<SudokuCell*> Col;
        GetUnsolvedInColumn(i, Col);
        SolveArray(Col);
    }
}

void SudokuBoard::SolveBySquares()
{
    for (coord_t i = 0; i < 8; i+=3)
    {
        for(coord_t j = 0; j < 8; j+=3)
        {
            std::vector<SudokuCell*> Square;
            GetUnsolvedInSquare({i,j}, Square);
            SolveArray(Square);
        }
    }
}

void SudokuBoard::SolveArray(std::vector<SudokuCell*>& InVector)
{
    std::map<num_t, std::vector<SudokuCell*>> map;
    GetUnsolvedMapFromArray(InVector, map);

    for (auto& Pair : map)
    {
        if (Pair.second.size() == 1)
        {
            AssingNumberAndHandle(Pair.second[0],Pair.first);
        }
    }
}

void SudokuBoard::FindObviousTuples()
{
}

void SudokuBoard::FindObviousTuplesInArray(std::vector<SudokuCell*>& InVector)
{
    if(InVector.size() < 4)
        return;

}

void DeletePossibleNumberForEntry(num_t Number, std::vector<SudokuCell*>& const Entry)
{
    for(auto Cell: Entry)
    {
        Cell->RemovePossibleNumber(Number);
    }
}

void SudokuBoard::AssingNumberAndHandle(SudokuCell* const Cell, const num_t Number)
{
    Cell->AssignNumber(Number);
    HandleCellAssigning(Cell);
    Cell->bAssigningHandled = true;
    bChangedOnPreviousCycle = true;
}

void SudokuBoard::HandleCellAssigning(SudokuCell* Cell)
{
    num_t AssignedNumber = Cell->GetAssignedNumber();
    CellCoordinates Coords = Cell->GetCoordinates();
    std::vector<SudokuCell*> Entry;
    GetRow(Coords.Y, Entry);
    DeletePossibleNumberForEntry(AssignedNumber, Entry);
    GetColumn(Coords.X, Entry);
    DeletePossibleNumberForEntry(AssignedNumber, Entry);
    GetSquare(Coords, Entry);
    DeletePossibleNumberForEntry(AssignedNumber,Entry);
}
