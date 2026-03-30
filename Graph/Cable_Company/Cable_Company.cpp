#include <iostream>
#include <limits.h>
#include <vector>

int main()
{
    int services;
    std::cin >> services;

    std::vector<std::vector<int>> cost(services, std::vector<int>(services, 0));

    for (int i = 0; i < services - 1; ++i)
    {
        for (int j = 0; j < services - i - 1; ++j)
        {
            int price;
            std::cin >> price;
            cost[i][i + j + 1] = price;
            cost[i + j + 1][i] = price;
        }
    }

    std::vector<int> min_weight(services, INT_MAX);
    std::vector<bool> in_mst(services, false);
    int totalCost = 0;

    min_weight[0] = 0;

    for (int i = 0; i < services; ++i)
    {
        int lowestIndex = -1;

        for (int j = 0; j < services; ++j)
        {
            if (!in_mst[j] && (lowestIndex == -1 || min_weight[j] < min_weight[lowestIndex]))
            {
                lowestIndex = j;
            }
        }

        in_mst[lowestIndex] = true;
        totalCost += min_weight[lowestIndex];

        for (int j = 0; j < services; ++j)
        {
            if (!in_mst[j] && lowestIndex != j)
            {
                if (cost[lowestIndex][j] < min_weight[j])
                {
                    min_weight[j] = cost[lowestIndex][j];
                }
            }
        }
    }

    std::cout << totalCost;
}