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
    int cities, roads, minCapacity = 0;
    std::cin >> cities >> roads;
    std::vector<std::vector<Node>> adj(cities, std::vector<Node>());

    for (int i = 0; i < roads; ++i)
    {
        int from, to, weight;
        std::cin >> from >> to >> weight;
        adj[from].push_back({to, weight});
        adj[to].push_back({from, weight});
    }

    for (int startNode = 0; startNode < cities; ++startNode)
    {
        std::priority_queue<Node> pq;
        std::vector<int> distance(cities, INT_MAX - 10000);
        distance[startNode] = 0;
        pq.push({startNode, 0});

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
                int next_to = edge.to;
                int next_weight = edge.weight;

                if (distance[to] + next_weight < distance[next_to])
                {
                    distance[next_to] = distance[to] + next_weight;
                    pq.push({next_to, distance[next_to]});
                }
            }
        }

        for (int i = 0; i < cities; ++i)
        {
            minCapacity = std::max(minCapacity, distance[i]);
        }
    }

    std::cout << minCapacity;
}