#include <iostream>
#include <vector>

int main()
{
    int N;
    std::cin >> N;
    std::vector<unsigned long long> DP(N + 1, 0);
    DP[0] = 1;

    for (int i = 1; i <= N; ++i)
    {
        for(int j = i; j <= N; ++j)
        {
            DP[j] += DP[j - i];
        }
    }

    std::cout << DP[N];
}