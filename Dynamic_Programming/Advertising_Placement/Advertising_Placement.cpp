#include <algorithm>
#include <iostream>
#include <limits.h>
#include <vector>

int main()
{
    int billboardsCount;
    std::cin >> billboardsCount;
    std::vector<int> customerSee(billboardsCount);
    std::vector<int> DP(billboardsCount, INT_MIN);

    for (int i = 0; i < billboardsCount; ++i)
    {
        std::cin >> customerSee[i];
    }

    DP[0] = customerSee[0];

    if (billboardsCount > 1)
    {
        DP[1] = std::max(customerSee[0], customerSee[1]);
    }

    for (int i = 2; i < billboardsCount; ++i)
    {
        DP[i] = std::max(DP[i - 1], DP[i - 2] + customerSee[i]);
    }

    std::cout << DP[billboardsCount - 1];
}