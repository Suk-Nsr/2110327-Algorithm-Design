#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>

const int INF = 1e9;

struct Edge
{
    int to, weight;
};

int main()
{
    int n, m, k;
    std::cin >> n >> m >> k;

    int destination;
    std::cin >> destination;

    std::vector<int> starts(k);

    for (int i = 0; i < k; ++i)
    {
        std::cin >> starts[i];
    }

    std::vector<std::vector<Edge>> adjacents(n);

    for (int i = 0; i < m; ++i)
    {
        int u, v, w;
        std::cin >> u >> v >> w;
        adjacents[v].push_back({u, w});
    }

    std::vector<int> distance(n, INF);
    std::vector<bool> inQueue(n, false);
    std::queue<int> queue;

    distance[destination] = 0;
    queue.push(destination);
    inQueue[destination] = true;

    while (!queue.empty())
    {
        int u = queue.front();
        queue.pop();
        inQueue[u] = false;

        for (const auto & edge : adjacents[u])
        {
            int v = edge.to;
            int weight = edge.weight;

            if (distance[v] > distance[u] + weight)
            {
                distance[v] = distance[u] + weight;

                if (!inQueue[v])
                {
                    queue.push(v);
                    inQueue[v] = true;
                }
            }
        }
    }

    int min_dist = INF;

    for (int i = 0; i < k; ++i)
    {
        int start_node = starts[i];
        if (distance[start_node] < min_dist)
        {
            min_dist = distance[start_node];
        }
    }

    std::cout << min_dist;
}