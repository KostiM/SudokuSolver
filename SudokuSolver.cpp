// SudokuSolver.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include "SudokuBoard.h"

int main(int argc, char* argv[])
{
    SudokuBoard Board;
    if(argc > 1)
    {
        Board.ReadContentFromFile(argv[1]);
    }
    Board.Print();
    std::cout << std::endl << std::endl;
    Board.Solve();
}
