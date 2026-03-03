#include <algorithm>
#include <iostream>
#include <limits.h>
#include <vector>

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    int dataAmount;
    std::cin >> dataAmount;
    std::vector<int> left(dataAmount);
    std::vector<int> right(dataAmount);
    
    std::vector<int> data(dataAmount);
    for (int i = 0; i < dataAmount; ++i)
    {
        std::cin >> data[i];
    }

    int leftGlobalMax = data[0];
    int leftCurrentMax = data[0];
    left[0] = leftGlobalMax;
    for (int i = 1; i <= dataAmount - 1; ++i)
    {
        leftCurrentMax = std::max(data[i], data[i] + leftCurrentMax);
        leftGlobalMax = std::max(leftCurrentMax, leftGlobalMax);
        left[i] = leftGlobalMax;
    }

    int rightGlobalMax = data[dataAmount - 1];
    int rightCurrentMax = data[dataAmount - 1];
    right[dataAmount - 1] = rightGlobalMax;
    for (int i = dataAmount - 2; i >= 0; --i)
    {
        rightCurrentMax = std::max(data[i], data[i] + rightCurrentMax);
        rightGlobalMax = std::max(rightCurrentMax, rightGlobalMax);
        right[i] = rightGlobalMax;
    }

    int maxValue = INT_MIN;
    for (int i = 0; i < dataAmount - 1; ++i)
    {
        maxValue = std::max(maxValue, left[i] + right[i + 1]);
    }

    if (maxValue > leftGlobalMax)
    {
        std::cout << maxValue;
    }
    else
    {
        std::cout << leftGlobalMax;
    }
}