#include <iostream>

int value11, value12, value21, value22;

int findValue(int matrixSize, long long row, long long column)
{
    if (matrixSize == 1)
    {
        if (row == 1 && column == 1)
        {
            return value11;
        }

        if (row == 1 && column == 2)
        {
            return value12;
        }

        if (row == 2 && column == 1)
        {
            return value21;
        }

        if (row == 2 && column == 2)
        {
            return value22;
        }
    }

    long long halfSize = 1LL << (matrixSize - 1);

    if (row <= halfSize && column <= halfSize)
    {
        return findValue(matrixSize - 1, row, column);
    }
    else if (row <= halfSize && column > halfSize)
    {
        return findValue(matrixSize - 1, column - halfSize, row);
    }
    else if (row > halfSize && column <= halfSize)
    {
        return -findValue(matrixSize - 1, row - halfSize, column);
    }
    else if (row > halfSize && column > halfSize)
    {
        return -findValue(matrixSize - 1, column - halfSize, row - halfSize);
    }
}

int main()
{
    int matrixSize, questionCount;
    std::cin >> matrixSize >> questionCount >> value11 >> value12 >> value21 >> value22;

    for (int i = 0; i < questionCount; ++i)
    {
        long long row, column;
        std::cin >> row >> column;
        std::cout << findValue(matrixSize, row, column) << "\n";
    }
}