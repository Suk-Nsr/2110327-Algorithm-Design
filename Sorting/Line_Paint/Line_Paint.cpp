#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int pairAmount;
    std::cin >> pairAmount;
    std::vector<std::pair<int, int>> interval(pairAmount);

    for (int i = 0; i < pairAmount; ++i)
    {
        std::cin >> interval[i].first >> interval[i].second;
    }

    std::sort(interval.begin(), interval.end());

    int rightMost = interval[0].second;

    std::cout << interval[0].first << " ";

    for (int i = 1; i < interval.size(); ++i)
    {
        if (rightMost + 1 < interval[i].first)
        {
            std::cout << rightMost << " " << interval[i].first << " ";
        }
        rightMost = std::max(rightMost, interval[i].second);
    }

    std::cout << rightMost;
}