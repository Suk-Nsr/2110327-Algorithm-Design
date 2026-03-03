#include <algorithm>
#include <iostream>
#include <limits.h>
#include <vector>

int main()
{
    int roadLength;
    std::cin >> roadLength;

    std::vector<int> customerSee(roadLength);
    for (int i = 0; i < roadLength; ++i)
    {
        std::cin >> customerSee[i];
    }

    std::vector<int> DP(roadLength, INT_MIN);
    DP[0] = customerSee[0];

    if (roadLength > 1)
    {
        DP[1] = std::max(customerSee[0], customerSee[1]);
    }

    if (roadLength > 2)
    {
        DP[2] = std::max({customerSee[0], customerSee[1], customerSee[2]});
    }

    for (int i = 3; i < roadLength; ++i)
    {
        DP[i] = std::max({DP[i - 3] + customerSee[i], DP[i - 2], DP[i - 1]});
    }

    std::cout << DP[roadLength - 1];
}