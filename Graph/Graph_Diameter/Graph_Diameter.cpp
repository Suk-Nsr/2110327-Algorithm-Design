#include <iostream>
#include <climits>
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
    int nodes, edges, maxValue = 0;
    std::cin >> nodes >> edges;
    std::vector<std::vector<Node>> adj(nodes, std::vector<Node>());

    for (int i = 0; i < edges; ++i)
    {
        int from, to, weight;
        std::cin >> from >> to >> weight;
        adj[from - 1].push_back({to - 1, weight});
    }

    for (int i = 0; i < nodes; ++i)
    {
        std::vector<int> distance(nodes, INT_MAX - 1000);
        std::priority_queue<Node> pq;
        distance[i] = 0;
        pq.push({i, 0});

        while (!pq.empty())
        {
            int to = pq.top().to;
            int weight = pq.top().weight;
            pq.pop();

            if (weight > distance[to])
            {
                continue;
            }

            for (Node & edge : adj[to])
            {
                if (distance[to] + edge.weight < distance[edge.to])
                {
                    distance[edge.to] = distance[to] + edge.weight;
                    pq.push({edge.to, distance[edge.to]});
                }
            }
        }

        for (int value : distance)
        {
            maxValue = std::max(value, maxValue);
        }
    }

    std::cout << maxValue;
}