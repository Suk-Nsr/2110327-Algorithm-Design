#include <iostream>
#include <vector>

int main()
{
    int chocolateLength, setSize;
    std::cin >> chocolateLength >> setSize;
    std::vector<int> set(setSize);
    std::vector<int> dp(chocolateLength + 1, 0);

    for (int i = 0; i < setSize; ++i)
    {
        std::cin >> set[i];
    }

    dp[0] = 1;

    for (int i = 1; i <= chocolateLength; ++i)
    {
        for (int size : set)
        {
            if (i >= size)
            {
                dp[i] = (dp[i] + dp[i - size]) % 1000003;
            }
        }
    }

    std::cout << dp[chocolateLength];
}