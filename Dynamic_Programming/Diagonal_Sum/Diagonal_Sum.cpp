#include <algorithm>
#include <iostream>
#include <limits.h>
#include <vector>

int main()
{
    int tableSize, answer = INT_MIN;
    std::cin >> tableSize;

    std::vector<std::vector<int>> valueTable(tableSize, std::vector<int>(tableSize));
    for (int i = 0; i < tableSize; ++i)
    {
        for (int j = 0; j < tableSize; ++j)
        {
            std::cin >> valueTable[i][j];
        }
    }

    std::vector<std::vector<int>> memo(tableSize + 1, std::vector<int>(tableSize + 1, INT_MIN + 1000));
    for (int i = tableSize - 1; i >= 0; --i)
    {
        for (int j = tableSize - 1; j >= 0; --j)
        {
            memo[i][j] = std::max(valueTable[i][j], valueTable[i][j] + memo[i + 1][j + 1]);

            if (memo[i][j] > answer)
            {
                answer = memo[i][j];
            }
        }
    }

    std::cout << answer;
}