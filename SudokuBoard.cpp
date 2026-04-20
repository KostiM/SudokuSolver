#include "SudokuBoard.h"
#include <iostream>
#include <fstream>
#include <string>
#include <array>

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
        std::cout << "Error: File " << FilePath << "couldn't be open.";
        return;
    }
    std::string Entry;
    size_t i = 0;
    size_t MaxCells = Cells.size();

    while (i < MaxCells && File >> Entry)
    {
        try 
        {
            Cells[i].AssignNumber(std::stoi(Entry));
            i++;
        }
        catch (const std::invalid_argument&)
        {
            std::cerr << "Warning: Invalid data '" << Entry << "' at index " << i << " skipped." << std::endl;
        }
    }

    if (i < MaxCells) {
        std::cout << "Warning: File ended sooner than expected. Read " << i << " cells." << std::endl;
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

        if (IsSolved()) {
            std::cout << "\nIt's done, here's the solution:\n";
            Print();
            return;
        }

        if (!bChangedOnPreviousCycle)
        {
            std::cout << "\nIt's stuck, can't be solved entirely, that's it for now: \n";
            std::cout << "Number of unsolved cells left: " << UnsolvedCellsNumber << "\n";
            Print();
            PrintCoordinatesAndPossibleNumbers();
            break;
        }
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
        if (Cell.GetAssignedNumber() == 0)
        {
            auto Coord = Cell.GetCoordinates();
            std::cout << '<' << Coord.X << ',' << Coord.Y << "> : ";
            
            const auto& Possible = Cell.GetPossibleNumbers();
            for (int num = 1; num <= 9; ++num)
            {
                if (Possible.test(num))
                {
                    std::cout << num << ' ';
                }
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
        if(Cell.GetPossibleCount() == 1)
        {
            for (int n = 1; n <= 9; ++n) {
                if (Cell.GetPossibleNumbers().test(n)) {
                    AssingNumberAndHandle(&Cell, n);
                }
            }
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

bool SudokuBoard::IsSolved()
{
    return !(UnsolvedCellsNumber > 0);
}

void SudokuBoard::SolveByRows()
{
    for(int i = 0; i < 9; ++i)
    {
        std::vector<SudokuCell*> Row;
        GetUnsolvedInRow(i,Row);
        SolveArray(Row);
    } 
}

void SudokuBoard::SolveByCols()
{
    for (int i = 0; i < 9; ++i)
    {
        std::vector<SudokuCell*> Col;
        GetUnsolvedInColumn(i, Col);
        SolveArray(Col);
    }
}

void SudokuBoard::SolveBySquares()
{
    for (coord_t i = 0; i < 9; i+=3)
    {
        for(coord_t j = 0; j < 9; j+=3)
        {
            std::vector<SudokuCell*> Square;
            GetUnsolvedInSquare({i,j}, Square);
            SolveArray(Square, true);
        }
    }
}

void SudokuBoard::SolveArray(std::vector<SudokuCell*>& InVector, bool IsSquare)
{
    std::array<std::vector<SudokuCell*>, 10> DigitCandidates;

    for (auto* Cell : InVector)
    {
        const auto& Possible = Cell->GetPossibleNumbers();
        for (int num = 1; num <= 9; ++num)
        {
            if (Possible.test(num))
            {
                DigitCandidates[num].push_back(Cell);
            }
        }
    }

    bool PreviouseSucceeded = false;
    for (int i = 1; i <=9; ++i)
    {
        if (DigitCandidates[i].size() == 1)
        {
            AssingNumberAndHandle(DigitCandidates[i][0], i);
            PreviouseSucceeded = true;
        }
    }

    if(!PreviouseSucceeded)
    {
        PreviouseSucceeded = SolveForNakedPairs(InVector);
    }

    if(!PreviouseSucceeded)
    {
        PreviouseSucceeded = SolveArrayForHiddenPairs(InVector, DigitCandidates);
    }

    if(!PreviouseSucceeded and IsSquare)
    {
       SolveForPointingPairs(InVector, DigitCandidates);
    }
}

bool SudokuBoard::SolveForNakedPairs(std::vector<SudokuCell*>& InVector)
{
    bool LocallyChanged = false;
    for (int i = 0; i < InVector.size(); ++i)
    {
        if (InVector[i]->GetPossibleNumbers().count() != 2)
        {
            continue;
        }
        for (int j = i + 1; j < InVector.size(); ++j)
        {
            if (InVector[i]->GetPossibleNumbers() != InVector[j]->GetPossibleNumbers())
            {
                continue;
            }
            NumSet PairMask = InVector[i]->GetPossibleNumbers();

            bool bLocalChange = false;
            for (auto* OtherCell : InVector)
            {
                if (OtherCell != InVector[i] && OtherCell != InVector[j])
                {
                    if (OtherCell->RemovePossibleNumbers(PairMask))
                    {
                        bChangedOnPreviousCycle = true;
                        LocallyChanged = true;
                        //std::cout << "--- NAKED PAIR FOUND! ---" << std::endl;
                    }
                }
            }
        }
    }
    return LocallyChanged;
}

bool SudokuBoard::SolveArrayForHiddenPairs(std::vector<SudokuCell*>& InVector, std::array<std::vector<SudokuCell*>, 10>& DigitCandidates)
{
    bool LocallyChanged = false;
    for (int i = 1; i <= 9; ++i)
    {
        if (DigitCandidates[i].size() == 2)
        {
            for (int j = i + 1; j <= 9; ++j)
            {
                if (DigitCandidates[j].size() == 2)
                {
                    if (DigitCandidates[i][0] == DigitCandidates[j][0] && DigitCandidates[i][1] == DigitCandidates[j][1])
                    {
                        bool bLocalChange = false;

                        NumSet PairMask;
                        PairMask.set(i);
                        PairMask.set(j);

                        for (int k = 0; k < 2; ++k)
                        {
                            SudokuCell* CellPtr = DigitCandidates[i][k];
                            if (CellPtr->KeepPossibleNumbers(PairMask))
                            {
                                bChangedOnPreviousCycle = true;
                                LocallyChanged = true;
                                //std::cout << "--- HIDDEN PAIR FOUND! ---" << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }
    return LocallyChanged;
}

void SudokuBoard::SolveForPointingPairs(std::vector<SudokuCell*>& InVector, std::array<std::vector<SudokuCell*>, 10>& DigitCandidates)
{
    for (int NumIndex = 1; NumIndex < 10; ++NumIndex)
    {
        if (DigitCandidates[NumIndex].size() > 3)
        {
            continue; 
        }

        coord_t CommonRow = INVALID_COORD;
        coord_t CommonCol = INVALID_COORD;
        NumSet Xs2Exlude;
        NumSet Ys2Exlude;
        for(int i = 0; i < DigitCandidates[NumIndex].size(); ++i)
        {
            CellCoordinates coord = DigitCandidates[NumIndex][i]->GetCoordinates();
            if( i == 0)
            {
                CommonRow = coord.Y;
                Ys2Exlude.set(coord.Y);
                CommonCol = coord.X;
                Xs2Exlude.set(coord.X);
            }
            else
            {
                if (CommonRow != coord.Y)
                {
                    CommonRow = INVALID_COORD;
                }

                if(CommonCol != coord.X)
                {
                    CommonCol = INVALID_COORD;
                }
            }
        }

        if(CommonRow != INVALID_COORD)
        {
            //std::cout << "--- POINTING PAIR FOUND! ---" << std::endl;
            for(coord_t i = 0; i < 9; ++i)
            {
                if ( Xs2Exlude.test(i) )
                {
                    continue;
                }
                SudokuCell* Cell = GetCell({i,CommonRow});
                bChangedOnPreviousCycle = bChangedOnPreviousCycle ||  Cell->RemovePossibleNumber(NumIndex);
            }
        }
        else if (CommonCol != INVALID_COORD)
        {
            //std::cout << "--- POINTING PAIR FOUND! ---" << std::endl;
            for (coord_t i = 0; i < 9; ++i)
            {
                if (Ys2Exlude.test(i))
                {
                    continue;
                }

                SudokuCell* Cell = GetCell({ CommonCol, i });
                bChangedOnPreviousCycle = bChangedOnPreviousCycle || Cell->RemovePossibleNumber(NumIndex);
            }
        }
    }
}

void SudokuBoard::AssingNumberAndHandle(SudokuCell* const Cell, const num_t Number)
{
    Cell->AssignNumber(Number);
    HandleCellAssigning(Cell);
    Cell->bAssigningHandled = true;
    bChangedOnPreviousCycle = true;
    UnsolvedCellsNumber--;
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

void SudokuBoard::DeletePossibleNumberForEntry(num_t Number, std::vector<SudokuCell*>& Entry)
{
    for (auto Cell : Entry)
    {
        if (Cell->RemovePossibleNumber(Number))
        {
            bChangedOnPreviousCycle = true;
        }
    }
}
