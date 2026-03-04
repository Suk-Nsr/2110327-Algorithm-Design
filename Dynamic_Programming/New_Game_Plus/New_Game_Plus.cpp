#include <iostream>
#include <vector>

int main()
{
    int row, column;
    std::cin >> row >> column;

    std::vector<std::vector<int>> accessible(row, std::vector<int>(column));
    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < column; ++j)
        {
            std::cin >> accessible[i][j];
        }
    }

    std::vector<std::vector<std::vector<int>>> DP(row, std::vector<std::vector<int>>(column, std::vector<int>(3, 0)));

    for (int i = 0; i < row; ++i)
    {
        if (accessible[i][1] == 1)
        {
            continue;
        }
        if (i + 1 < row && accessible[i + 1][0] == 0)
        {
            DP[i][1][0] = 1;
        }
        if (accessible[i][0] == 0)
        {
            DP[i][1][1] = 1;
        }
        if (i - 1 >= 0 && accessible[i - 1][0] == 0)
        {
            DP[i][1][2] = 1;
        }
    }

    for (int i = 2; i < column; ++i)
    {
        for (int j = 0; j < row; ++j)
        {
            if (accessible[j][i] == 1)
            {
                continue;
            }

            if (j + 1 < row)
            {
                DP[j][i][0] = (DP[j + 1][i - 1][1] + DP[j + 1][i - 1][2]) % 100000007;
            }

            DP[j][i][1] = (DP[j][i - 1][0] + DP[j][i - 1][2]) % 100000007;

            if (j - 1 >= 0)
            {
                DP[j][i][2] = (DP[j - 1][i - 1][0] + DP[j - 1][i - 1][1]) % 100000007;
            }
        }
    }

    long long answer = 0;
    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            answer = (answer + DP[i][column - 1][j]) % 100000007;
        }
    }

    std::cout << answer;
}