#include <algorithm>
#include <iostream>
#include <limits.h>
#include <iomanip>
#include <vector>

struct Item
{
    double price;
    double weight;
    double ratio;
};

bool compareItems(Item & a, Item & b)
{
    return a.ratio > b.ratio;
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    double maxWeight;
    int itemAmount;
    std::cin >> maxWeight >> itemAmount;

    std::vector<Item> items(itemAmount);

    for (int i = 0; i < itemAmount; ++i)
    {
        std::cin >> items[i].price;
    }

    for (int i = 0; i < itemAmount; ++i)
    {
        std::cin >> items[i].weight;

        if (items[i].weight > 0)
        {
            items[i].ratio = items[i].price / items[i].weight;
        }
        else
        {
            items[i].ratio = INT_MAX;
        }
    }

    std::sort(items.begin(), items.end(), compareItems);

    double currentPrice = 0, currentWeight = 0;

    for (int i = 0; i < itemAmount; ++i)
    {
        if ((currentWeight + items[i].weight) <= maxWeight)
        {
            currentWeight += items[i].weight;
            currentPrice += items[i].price;
        }
        else
        {
            currentPrice += items[i].ratio * (maxWeight - currentWeight);
            break;
        }
    }

    std::cout << std::fixed << std::setprecision(4) << currentPrice;
}