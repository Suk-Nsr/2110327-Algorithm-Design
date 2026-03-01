#include <iostream>
#include <vector>

int bino(int N, int K, std::vector<std::vector<int>> memory)
{
    for (int i = 0; i <= N; ++i)
    {
        memory[i][0] = 1;
        memory[i][i] = 1;
    }

    for (int i = 2; i <= N; ++i)
    {
        for (int j = 1; j < i; ++j)
        {
            memory[i][j] = memory[i - 1][j - 1] + memory[i - 1][j];
        }
    }

    return memory[N][K];
}

int main()
{
    int N, K;
    std::cin >> N >> K;
    std::vector<std::vector<int>> memory(N + 1, std::vector<int>(N + 1, -1));
    std::cout << bino(N, K, memory);
}