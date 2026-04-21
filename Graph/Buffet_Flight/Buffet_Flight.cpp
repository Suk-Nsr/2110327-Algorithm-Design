#include <iostream>
#include <vector>
#include <queue>

struct Node
{
    int to;
    int weight;

    bool operator < (const Node & other) const
    {
        return weight > other.weight;
    }
};

int main()
{
    int airports, routes;
    std::cin >> airports >> routes;

    std::vector<int> inCost(airports);
    std::vector<int> outCost(airports);
    std::vector<std::vector<Node>> adj(airports, std::vector<Node>());

    for (int i = 0; i < airports; ++i)
    {
        std::cin >> inCost[i];
    }

    for (int i = 0; i < airports; ++i)
    {
        std::cin >> outCost[i];
    }

    for (int i = 0; i < routes; ++i)
    {
        int from, to;
        std::cin >> from >> to;
        adj[from].push_back({to, inCost[to] + outCost[from]});
    }

    std::vector<int> cost(airports, -1);
    cost[0] = 0;
    std::priority_queue<Node> pq;
    pq.push({0, 0});

    while (!pq.empty())
    {
        int to = pq.top().to;
        int weight = pq.top().weight;
        pq.pop();

        if (weight > cost[to])
        {
            continue;
        }

        for (Node & edge : adj[to])
        {
            int next_to = edge.to;
            int next_weight = edge.weight;

            if (cost[to] + next_weight < cost[next_to] || cost[next_to] == -1)
            {
                cost[next_to] = cost[to] + next_weight;
                pq.push({next_to, cost[next_to]});
            }
        }
    }

    for (int & subCost : cost)
    {
        std::cout << subCost << " ";
    }
}