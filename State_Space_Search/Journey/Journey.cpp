#include <algorithm>
#include <iostream>
#include <climits>
#include <vector>

struct Edge
{
    int to;
    int weight;

    bool operator > (const Edge & other) const
    {
        return weight > other.weight;
    }
};

void DFS(std::vector<std::vector<Edge>> & adj, std::vector<bool> & visited, std::vector<int> & maxHap, int & maxHappiness, int & mapSize, int currentHappiness, int potentialHappiness, int currentCity, int count)
{
    if (currentCity == mapSize - 1)
    {
        if (count == mapSize)
        {
            maxHappiness = std::max(maxHappiness, currentHappiness);
        }
        return;
    }

    if (currentHappiness + potentialHappiness <= maxHappiness)
    {
        return;
    }

    for (auto edge : adj[currentCity])
    {
        if (visited[edge.to] == false)
        {
            if (edge.to == mapSize - 1 && count < mapSize - 1)
            {
                continue;
            }

            visited[edge.to] = true;
            DFS(adj, visited, maxHap, maxHappiness, mapSize, currentHappiness + edge.weight, potentialHappiness - maxHap[currentCity], edge.to, count + 1);
            visited[edge.to] = false;
        }
    }
}

int main()
{
    int mapSize, initialPotential = 0, maxHappiness = INT_MIN;
    std::cin >> mapSize;
    std::vector<bool> visited(mapSize, false);
    std::vector<int> maxHap(mapSize, INT_MIN);
    std::vector<std::vector<int>> map(mapSize, std::vector<int>(mapSize));
    std::vector<std::vector<Edge>> adj(mapSize);

    visited[0] = true;

    for (int i = 0; i < mapSize; ++i)
    {
        for (int j = 0; j < mapSize; ++j)
        {
            std::cin >> map[i][j];
            if (i != j)
            {
                maxHap[i] = std::max(maxHap[i], map[i][j]);
            }
        }

        if (i != mapSize - 1)
        {
            initialPotential += maxHap[i];
        }
    }

    for (int i = 0; i < mapSize; ++i)
    {
        for (int j = 0; j < mapSize; ++j)
        {
            if (i != j)
            {
                adj[i].push_back({j, map[i][j]});
            }
        }

        std::sort(adj[i].begin(), adj[i].end(), std::greater<Edge>());
    }

    DFS(adj, visited, maxHap, maxHappiness, mapSize, 0, initialPotential, 0, 1);

    std::cout << maxHappiness;
}