#include <vector>
#include <iostream>
#include <limits.h>
#include <algorithm>

int rowCount, columnCount;

int calculateMaxScore(std::vector<std::vector<int>> & scoreTable, std::vector<std::vector<int>> & scoreMaxingMem, int currentRow, int currentColumn)
{
    if (currentRow < 1 || currentColumn < 1)
    {
        return INT_MIN;
    }

    if (currentRow == 1 && currentColumn == 1)
    {
        return scoreTable[1][1];
    }

    if (scoreMaxingMem[currentRow][currentColumn] != -1)
    {
        return scoreMaxingMem[currentRow][currentColumn];
    }

    int scoreFromTop = calculateMaxScore(scoreTable, scoreMaxingMem, currentRow - 1, currentColumn) + scoreTable[currentRow][currentColumn];
    int scoreFromLeft = calculateMaxScore(scoreTable, scoreMaxingMem, currentRow, currentColumn - 1) + scoreTable[currentRow][currentColumn];
    int scoreFromDiagonal = calculateMaxScore(scoreTable, scoreMaxingMem, currentRow - 1, currentColumn - 1) + (scoreTable[currentRow][currentColumn] * 2);

    scoreMaxingMem[currentRow][currentColumn] = std::max({scoreFromTop, scoreFromLeft, scoreFromDiagonal});
    
    return scoreMaxingMem[currentRow][currentColumn];
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    std::cin >> rowCount >> columnCount;
    std::vector<std::vector<int>> scoreTable(rowCount + 1, std::vector<int>(columnCount + 1));
    std::vector<std::vector<int>> scoreMaxingMem(rowCount + 1, std::vector<int>(columnCount + 1, -1));

    for (int i = 1; i <= rowCount; ++i)
    {
        for (int j = 1; j <= columnCount; ++j)
        {
            std::cin >> scoreTable[i][j];
        }
    }

    std::cout << calculateMaxScore(scoreTable, scoreMaxingMem, rowCount, columnCount);
}