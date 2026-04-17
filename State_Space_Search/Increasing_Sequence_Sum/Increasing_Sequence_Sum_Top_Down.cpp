#include <iostream>
#include <vector>

int solve(std::vector<std::vector<int>> & memo, int remaining, int last)
{
    if (remaining == 0)
    {
        return 1;
    }

    if (memo[remaining][last] != -1)
    {
        return memo[remaining][last];
    }

    int count = 0;

    for (int i = last; i <= remaining; ++i)
    {
        count += solve(memo, remaining - i, i);
    }

    return memo[remaining][last] = count;
}

int main()
{
    int N;
    std::cin >> N;
    std::vector<std::vector<int>> memo(N + 1, std::vector<int>(N + 1, -1));
    std::cout << solve(memo, N, 1);
}