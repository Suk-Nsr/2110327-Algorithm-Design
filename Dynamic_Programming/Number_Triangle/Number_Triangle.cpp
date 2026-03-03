#include <algorithm>
#include <iostream>
#include <limits.h>
#include <vector>

int main()
{
    int level;
    std::cin >> level;
    std::vector<std::vector<int>> data(level, std::vector<int>(level));
    std::vector<std::vector<int>> DP(level, std::vector<int>(level));

    for (int i = 0; i < level; ++i)
    {
        for (int j = 0; j < i + 1; ++j)
        {
            std::cin >> data[i][j];
        }
    }

    for (int i = level - 1; i >= 0; --i)
    {
        for (int j = 0; j <= i; ++j)
        {
            if (i == level - 1)
            {
                DP[i][j] = data[i][j];
            }
            else
            {
                DP[i][j] = std::max(data[i][j] + DP[i + 1][j], data[i][j] + DP[i + 1][j + 1]);
            }
        }
    }

    std::cout << DP[0][0];
}