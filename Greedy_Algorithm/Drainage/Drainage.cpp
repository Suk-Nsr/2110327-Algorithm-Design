#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    int drainageAmounts, boardLength, boardCover = 0, boardCount = 0;
    std::cin >> drainageAmounts >> boardLength;
    std::vector<int> drainages(drainageAmounts);

    for (int i = 0; i < drainageAmounts; ++i)
    {
        std::cin >> drainages[i];
    }

    std::sort(drainages.begin(), drainages.end());

    for (int i = 0; i < drainageAmounts; ++i)
    {
        if (boardCover > drainages[drainageAmounts - 1])
        {
            break;
        }
        if (drainages[i] <= boardCover)
        {
            continue;
        }
        else
        {
            boardCover = drainages[i] + boardLength - 1;
            ++boardCount;
        }
    }

    std::cout << boardCount;
}