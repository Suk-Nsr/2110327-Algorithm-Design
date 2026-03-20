#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>

struct event
{
    int day;
    int type;
    int label;
};

bool compareDay(event & a, event & b)
{
    return a.day < b.day;
}

int main()
{
    int plantAmounts, storesAmounts, days;
    std::cin >> plantAmounts >> storesAmounts >> days;
    std::vector<event> events(days);
    std::queue<int> produced;
    std::queue<int> needed;

    for (int i = 0; i < days; ++i)
    {
        std::cin >> events[i].day >> events[i].type >> events[i].label;
    }

    std::sort(events.begin(), events.end(), compareDay);

    for (int i = 0; i < days; ++i)
    {
        if (events[i].type == 0)
        {
            if (needed.empty())
            {
                produced.push(events[i].label);
                std::cout << 0 << "\n";
            }
            else
            {
                std::cout << needed.front() << "\n";
                needed.pop();
            }
        }
        else
        {
            if (produced.empty())
            {
                needed.push(events[i].label);
                std::cout << 0 << "\n";
            }
            else
            {
                std::cout << produced.front() << "\n";
                produced.pop();
            }
        }
    }
}