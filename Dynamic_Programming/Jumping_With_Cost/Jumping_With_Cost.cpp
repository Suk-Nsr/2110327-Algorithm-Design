#include <algorithm>
#include <iostream>
#include <limits.h>
#include <vector>

int main()
{
    int boardLength, jumpCostLength;
    std::cin >> boardLength >> jumpCostLength;

    std::vector<int> board(boardLength);
    for (int i = 0; i < boardLength; ++i)
    {
        std::cin >> board[i];
    }

    std::vector<int> jumpCost(jumpCostLength);
    for (int i = 0; i < jumpCostLength; ++i)
    {
        std::cin >> jumpCost[i];
    }

    std::vector<int> DP(boardLength, INT_MIN + 1001);
    DP[0] = board[0];
    for (int i = 1; i < boardLength; ++i)
    {
        for (int j = 0; j < jumpCostLength; ++j)
        {
            if (i > j)
            {
                DP[i] = std::max(DP[i], DP[i - j - 1] + board[i] - jumpCost[j]);
            }
        }
    }

    std::cout << DP[boardLength - 1];
}