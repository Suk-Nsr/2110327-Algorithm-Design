#include <iostream>
#include <vector>
#include <queue>

int main()
{
    int outposts, roads, distance, currentDistance = 0;
    std::cin >> outposts >> roads >> distance;

    std::vector<std::vector<int>> adjacent(outposts + 1, std::vector<int>());
    std::vector<bool> visited(outposts + 1, false);
    std::queue<int> queue;
    queue.push(0);
    visited[0] = true;

    for (int i = 0; i < roads; ++i)
    {
        int from, to;
        std::cin >> from >> to;
        adjacent[from].push_back(to);
        adjacent[to].push_back(from);
    }

    while (currentDistance != distance)
    {
        int iteration = queue.size();

        for (int i = 0; i < iteration; ++i)
        {
            for (int neighbor : adjacent[queue.front()])
            {
                if (visited[neighbor] == false)
                {
                    queue.push(neighbor);
                    visited[neighbor] = true;
                }
            }

            queue.pop();
        }

        ++currentDistance;
    }

    std::cout << queue.size();
}