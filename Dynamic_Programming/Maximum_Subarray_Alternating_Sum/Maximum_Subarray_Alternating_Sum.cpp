#include <iostream>
#include <limits.h>
#include <vector>

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int arraySize;
    std::cin >> arraySize;
    std::vector<long long> array(arraySize);

    for (int i = 0; i < arraySize; ++i)
    {
        std::cin >> array[i];
    }

    long long currentPositive = array[0];
    long long currentNegative = -100000000;
    long long answer = currentPositive;

    for (int i = 1; i < arraySize; ++i)
    {
        long long nextPositive = std::max(array[i], currentNegative + array[i]);
        long long nextNegative = currentPositive - array[i];

        currentPositive = nextPositive;
        currentNegative = nextNegative;

        answer = std::max(answer, std::max(currentPositive, currentNegative));
    }

    std::cout << answer;
}