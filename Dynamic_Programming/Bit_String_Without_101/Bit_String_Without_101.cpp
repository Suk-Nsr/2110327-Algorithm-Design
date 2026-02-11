#include <iostream>
#include <vector>

int main()
{
    int stringLength;
    std::cin >> stringLength;

    int MOD = 100000007;

    std::vector<std::vector<int>> states(stringLength + 1, std::vector<int>(3));

    states[1][0] = 1;
    states[1][1] = 1;
    states[1][2] = 0;

    for (int i = 2; i <= stringLength; ++i)
    {
        states[i][0] = (states[i - 1][0] + states[i - 1][2]) % MOD;
        states[i][1] = (states[i - 1][0] + states[i - 1][1]) % MOD;
        states[i][2] = states[i - 1][1] % MOD;
    }

    int answer = (states[stringLength][0] + states[stringLength][1] + states[stringLength][2]) % MOD;

    std::cout << answer;
}