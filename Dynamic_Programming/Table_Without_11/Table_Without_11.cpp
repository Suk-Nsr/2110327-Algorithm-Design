#include <iostream>
#include <vector>

int main()
{
    int columnCount;
    std::cin >> columnCount;
    std::vector<std::vector<long long>> DP(columnCount, std::vector<long long>(3));

    DP[0][0] = 1;
    DP[0][1] = 1;
    DP[0][2] = 1;

    for (int i = 1; i < columnCount; ++i)
    {
        DP[i][0] = (DP[i - 1][0] + DP[i - 1][1] + DP[i - 1][2]) % 100000007;
        DP[i][1] = (DP[i - 1][0] + DP[i - 1][2]) % 100000007;
        DP[i][2] = (DP[i - 1][0] + DP[i - 1][1]) % 100000007;
    }

    std::cout << (DP[columnCount - 1][0] + DP[columnCount - 1][1] + DP[columnCount - 1][2]) % 100000007;
}