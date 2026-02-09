#include <iostream>

int value;

int findValue(int rowStart, int rowStop, int columnStart, int columnStop, int row, int column)
{
    int rowMid = (rowStart + rowStop) >> 1;
    int columnMid = (columnStart + columnStop) >> 1;

    if (rowStart == rowStop)
    {
        return value;
    }

    if (row <= rowMid && column <= columnMid)
    {
        return findValue(rowStart, rowMid, columnStart, columnMid, row, column);
    }
    if (row <= rowMid && column > columnMid)
    {
        return findValue(rowStart, rowMid, columnMid + 1, columnStop, row, column) - 1;
    }
    if (row > rowMid && column <= columnMid)
    {
        return findValue(rowMid + 1, rowStop, columnStart, columnMid, row, column) + 1;
    }
    if (row > rowMid && column > columnMid)
    {
        return findValue(rowMid + 1, rowStop, columnMid + 1, columnStop, row, column);
    }
}

int main()
{
    int matrixSize = 1, shiftBitBy;
    std::cin >> shiftBitBy >> value;
    matrixSize = matrixSize << shiftBitBy;

    for (int i = 0; i < matrixSize; ++i)
    {
        for (int j = 0; j < matrixSize; ++j)
        {
            std::cout << findValue(0, matrixSize - 1, 0, matrixSize - 1, i, j) << " ";
        }
        
        std::cout << "\n";
    }
}