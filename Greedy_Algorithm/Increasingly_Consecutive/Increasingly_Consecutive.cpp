#include <algorithm>
#include <iostream>
#include <unordered_map>

int main()
{
    int cardAmount;
    long long score = 0;
    std::cin >> cardAmount;
    std::unordered_map<long long, long long> numAmount;

    for (int i = 0; i < cardAmount; ++i)
    {
        long long cardValue;
        std::cin >> cardValue;
        ++numAmount[cardValue];
    }

    for (auto value : numAmount)
    {
        if (value.second != 1)
        {
            score += value.second * value.second;
        }
    }

    std::cout << score;
}