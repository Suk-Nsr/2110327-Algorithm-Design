#include <iostream>
#include <vector>

int maxScoreCalculation(std::vector<std::vector<int>> &scoreTable, int currentRow, int currentColumn, int & targetRow, int & targetColumn, int cumulativeScore, bool diagonal)
{
    if (diagonal)
    {
        cumulativeScore += scoreTable[currentRow][currentColumn] * 2;
    }
    else
    {
        cumulativeScore += scoreTable[currentRow][currentColumn];
    }

    if (currentRow == targetRow && currentColumn == targetColumn)
    {
        return cumulativeScore;
    }

    int score1 = 0, score2 = 0, score3 = 0;

    if (currentRow < targetRow)
    {
        score1 = maxScoreCalculation(scoreTable, currentRow + 1, currentColumn, targetRow, targetColumn, cumulativeScore, false);
    }

    if (currentColumn < targetColumn)
    {
        score2 = maxScoreCalculation(scoreTable, currentRow, currentColumn + 1, targetRow, targetColumn, cumulativeScore, false);
    }

    if (currentRow < targetRow && currentColumn < targetColumn)
    {
        score3 = maxScoreCalculation(scoreTable, currentRow + 1, currentColumn + 1, targetRow, targetColumn, cumulativeScore, true);
    }

    if (score1 >= score2 && score1 >= score3)
    {
        return score1;
    }

    if (score2 >= score1 && score2 >= score3)
    {
        return score2;
    }

    if (score3 >= score1 && score3 >= score2)
    {
        return score3;
    }
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int row, column;
    std::cin >> row >> column;

    std::vector<std::vector<int>> scoreTable(row, std::vector<int>(column));

    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < column; ++j)
        {
            std::cin >> scoreTable[i][j];
        }
    }

    --row;
    --column;

    std::cout << maxScoreCalculation(scoreTable, 0, 0, row, column, 0, false);
}