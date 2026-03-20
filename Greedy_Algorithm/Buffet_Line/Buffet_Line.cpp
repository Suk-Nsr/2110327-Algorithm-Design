#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    int foodAvailable, lightRadius, tableLength, currentLightPos = 0, lightPlaced = 0;
    std::cin >> foodAvailable >> lightRadius >> tableLength;

    std::vector<int> foodPosition(foodAvailable);

    for (int i = 0; i < foodAvailable; ++i)
    {
        std::cin >> foodPosition[i];
    }

    std::sort(foodPosition.begin(), foodPosition.end());

    for (int i = 0; i < foodAvailable; ++i)
    {
        if (currentLightPos > tableLength)
        {
            break;
        }
        else
        {
            if (foodPosition[i] <= currentLightPos)
            {
                continue;
            }
            else
            {
                currentLightPos = foodPosition[i] + 2 * lightRadius;
                ++lightPlaced;
            }
        }
    }

    std::cout << lightPlaced;
}