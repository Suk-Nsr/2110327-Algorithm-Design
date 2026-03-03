#include <iostream>
#include <vector>

int main()
{
    int stringLength, space;
    std::cin >> stringLength >> space;
    std::vector<int> DP(stringLength + 1);

    DP[0] = 1;
    DP[1] = 2;

    for (int i = 2; i <= stringLength; ++i)
    {
        if (i < space)
        {
            DP[i] = i + 1;
        }
        else
        {
            DP[i] = (DP[i - 1] + DP[i - space]) % 100000007;
        }
    }

    std::cout << DP[stringLength];
}