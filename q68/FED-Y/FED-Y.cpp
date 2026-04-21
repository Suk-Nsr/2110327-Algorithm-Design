#include <iostream>
#include <climits>
#include <vector>
#include <queue>

struct Node
{
    int to;
    long long weight;

    bool operator < (const Node & other) const
    {
        return weight > other.weight;
    }
};

int main()
{
    int cities, roads;
    long long maxTime = 0;
    std::cin >> cities >> roads;
    std::vector<std::vector<Node>> adj(cities, std::vector<Node>());
    std::vector<long long> distance(cities, LONG_LONG_MAX - 1000000000);
    distance[0] = 0;
    std::priority_queue<Node> pq;
    pq.push({0, 0});

    for (int i = 0; i < roads; ++i)
    {
        int from, to, weight;
        std::cin >> from >> to >> weight;
        adj[from - 1].push_back({to - 1, weight});
        adj[to - 1].push_back({from - 1, weight});
    }

    while (!pq.empty())
    {
        int to = pq.top().to;
        long long weight = pq.top().weight;
        pq.pop();

        if (weight > distance[to])
        {
            continue;
        }

        for (Node & edge : adj[to])
        {
            int next_to = edge.to;
            long long next_weight = edge.weight;

            if (distance[to] + next_weight < distance[next_to])
            {
                distance[next_to] = distance[to] + next_weight;
                pq.push({next_to, distance[next_to]});
            }
        }
    }
    
    for (int i = 1; i < cities; ++i)
    {
        maxTime = std::max(maxTime, distance[i]);
    }

    for (int i = 1; i < cities; ++i)
    {
        std::cout << maxTime - distance[i] << "\n";
    }
}