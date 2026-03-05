#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

int main()
{
    int groupCount, sumAll = 0, subSum = 0, halfSum;
    std::cin >> groupCount;
    std::vector<std::pair<int, int>> group(groupCount);
    for (int i = 0; i < groupCount; ++i)
    {
        int subGroupCount;
        std::cin >> subGroupCount;
        sumAll += subGroupCount;
        group[i] = std::make_pair(subGroupCount, i + 1);
    }
    halfSum = sumAll / 2;
    std::sort(group.begin(), group.end());
    for (int i = groupCount - 1; i >= 0; --i)
    {
        subSum += group[i].first;
        if (subSum > halfSum)
        {
            std::cout << groupCount - i << "\n";
            for (int j = i; j < groupCount; ++j)
            {
                std::cout << group[j].second << " ";
            }
            break;
        }
    }
}