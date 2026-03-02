#include <iostream>

int main()
{
    int c;
    std::cin >> c;

    long long stateA = 1;
    long long stateB = 1;
    long long stateC = 0;
    long long MOD = 100000007;

    for (int i = 2; i <= c; ++i)
    {
        long long nextStateA = (stateA + 2 * stateC) % MOD;
        long long nextStateB = (stateA + stateB + stateC) % MOD;
        long long nextStateC = (stateB) % MOD;

        stateA = nextStateA;
        stateB = nextStateB;
        stateC = nextStateC;
    }

    long long answer = (stateA + 2 * stateB + 2 * stateC) % MOD;
    std::cout << answer;
}