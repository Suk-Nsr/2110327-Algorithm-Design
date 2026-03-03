#include <iostream>
#include <vector>

int main()
{
    int row, column, pairAmount;
    std::cin >> row >> column >> pairAmount;
    std::vector<std::vector<int>> data(row, std::vector<int>(column));
    std::vector<std::vector<int>> prefixSum(row + 1, std::vector<int>(column + 1));

    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < column; ++j)
        {
            std::cin >> data[i][j];
        }
    }

    for (int i = 1; i <= row; ++i)
    {
        for (int j = 1; j <= column; ++j)
        {
            int leftPrefix = 0, topPrefix = 0, diagPrefix = 0;

            if (i >= 1)
            {
                topPrefix = prefixSum[i - 1][j];
            }

            if (j >= 1)
            {
                leftPrefix = prefixSum[i][j - 1];
            }

            if (i >= 1 && j >= 1)
            {
                diagPrefix = prefixSum[i - 1][j - 1];
            }

            prefixSum[i][j] = topPrefix + leftPrefix - diagPrefix + data[i - 1][j - 1];
        }
    }

    for (int i = 0; i < pairAmount; ++i)
    {
        int firstRow, firstColumn, secondRow, secondColumn;
        std::cin >> firstRow >> firstColumn >> secondRow >> secondColumn;

        ++firstRow;
        ++firstColumn;
        ++secondRow;
        ++secondColumn;

        std::cout << prefixSum[secondRow][secondColumn] - prefixSum[firstRow - 1][secondColumn] - prefixSum[secondRow][firstColumn - 1] + prefixSum[firstRow - 1][firstColumn - 1] << "\n";
    }
}