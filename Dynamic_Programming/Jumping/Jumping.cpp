#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    int arraySize;
    std::cin >> arraySize;
    std::vector<int> array(arraySize + 1);

    for (int i = 1; i <= arraySize; ++i)
    {
        std::cin >> array[i];
    }

    std::vector<int> prefixSum(arraySize + 1);

    prefixSum[1] = array[1];

    if (arraySize >= 2)
    {
        prefixSum[2] = prefixSum[1] + array[2];
    }

    if (arraySize >= 3)
    {
        prefixSum[3] = std::max(prefixSum[1], prefixSum[2]) + array[3];
    }

    for (int i = 4; i <= arraySize; ++i)
    {
        int prev_max = std::max(prefixSum[i - 1], std::max(prefixSum[i - 2], prefixSum[i - 3]));
        prefixSum[i] = array[i] + prev_max;
    }

    std::cout << prefixSum[arraySize];
}