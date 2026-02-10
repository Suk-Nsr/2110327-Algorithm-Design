#include <iostream>
#include <limits.h>
#include <vector>

int main()
{
    int arraySize;
    std::cin >> arraySize;
    std::vector<int> prefixSum(2026, INT_MAX);

    int currentSum = 0;
    int maxValue = INT_MIN;

    for (int i = 0; i < arraySize; ++i)
    {
        int value;
        std::cin >> value;

        int prevSum = currentSum;
        currentSum += value;

        int index = value + 1000;

        if (prevSum < prefixSum[index])
        {
            prefixSum[index] = prevSum;
        }

        int currentAnswer = currentSum - prefixSum[index];

        if (currentAnswer > maxValue)
        {
            maxValue = currentAnswer;
        }
    }

    std::cout << maxValue;
    
}