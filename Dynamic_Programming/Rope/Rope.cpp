#include <iostream>
#include <algorithm>
#include <vector>
#include <limits.h>

int main()
{
    int ropeLength, a, b, c;
    std::cin >> ropeLength >> a >> b >> c;
    std::vector<int> DP(ropeLength + 1, INT_MIN);
    DP[0] = 0;

    for (int i = 1; i <= ropeLength; ++i)
    {
        if (i >= a)
        {
            DP[i] = std::max(DP[i], DP[i - a] + 1);
        }

        if (i >= b)
        {
            DP[i] = std::max(DP[i], DP[i - b] + 1);
        }

        if (i >= c)
        {
            DP[i] = std::max(DP[i], DP[i - c] + 1);
        }
    }

    std::cout << DP[ropeLength];
}