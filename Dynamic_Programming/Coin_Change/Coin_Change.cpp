#include <algorithm>
#include <iostream>
#include <limits.h>
#include <vector>

int main()
{
    int denominationCount, targetMoney;
    std::cin >> denominationCount >> targetMoney;
    std::vector<int> denominations(denominationCount);
    std::vector<int> DP(targetMoney + 1, INT_MAX);
    DP[0] = 0;

    for (int i = 0; i < denominationCount; ++i)
    {
        std::cin >> denominations[i];
    }

    for (int i = 1; i <= targetMoney; ++i)
    {
        for (int j = denominationCount - 1; j >= 0; --j)
        {
            if (i >= denominations[j])
            {
                DP[i] = std::min(DP[i], DP[i - denominations[j]] + 1);
            }
        }
    }

    std::cout << DP[targetMoney];
}