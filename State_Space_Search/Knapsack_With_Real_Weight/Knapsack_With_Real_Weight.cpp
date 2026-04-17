#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

double maxValue = 0.0;
double maxWeight;
int items;

struct Item
{
    double value;
    double weight;
    double ratio;

    bool operator > (const Item & other) const
    {
        return ratio > other.ratio;
    }
};

double getBound(std::vector<Item> & item, int index, double currentWeight, double currentValue)
{
    double weight = currentWeight;
    double value = currentValue;

    while(index < items && weight + item[index].weight <= maxWeight)
    {
        weight += item[index].weight;
        value += item[index].value;
        ++index;
    }

    if (index < items)
    {
        value += (maxWeight - weight) * item[index].ratio;
    }

    return value;
}

void DFS(std::vector<Item> & item, int index, double currentWeight, double currentValue)
{
    if (currentValue > maxValue)
    {
        maxValue = currentValue;
    }

    if (index == items)
    {
        return;
    }

    if (getBound(item, index, currentWeight, currentValue) <= maxValue)
    {
        return;
    }

    if (currentWeight + item[index].weight <= maxWeight)
    {
        DFS(item, index + 1, currentWeight + item[index].weight, currentValue + item[index].value);
    }

    DFS(item, index + 1, currentWeight, currentValue);
}

int main()
{
    std::cin >> maxWeight >> items;

    std::vector<Item> item(items);

    for (int i = 0; i < items; ++i)
    {
        std::cin >> item[i].value;
    }

    for (int i = 0; i < items; ++i)
    {
        std::cin >> item[i].weight;
        item[i].ratio = item[i].value / item[i].weight;
    }

    std::sort(item.begin(), item.end(), std::greater<Item>());

    DFS(item, 0, 0.0, 0.0);

    std::cout << std::fixed << std::setprecision(4) << maxValue;
}