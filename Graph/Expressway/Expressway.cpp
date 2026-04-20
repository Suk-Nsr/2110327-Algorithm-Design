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

int runDijkstra(std::vector<std::vector<Node>> & adj, int n)
{
    std::vector<int> distance(n, INT_MAX - 1000000);
    std::priority_queue<Node> pq;

    distance[0] = 0;
    pq.push({0, 0});

    while (!pq.empty())
    {
        int to = pq.top().to;
        int weight = pq.top().weight;
        pq.pop();

        if (weight > distance[to])
        {
            continue;
        }

        if (to == 1)
        {
            return distance[1];
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

    return distance[1];
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int years, initialPrice;
    std::cin >> years >> initialPrice;

    std::vector<std::vector<Node>> adj(years);

    adj[0].push_back({1, initialPrice});
    adj[1].push_back({0, initialPrice});

    for (int i = 2; i < years; ++i)
    {
        int highwayCount;
        std::cin >> highwayCount;

        for (int j = 0; j < highwayCount; ++j)
        {
            int to, weight;
            std::cin >> to >> weight;
            adj[i].push_back({to - 1, weight});
            adj[to - 1].push_back({i, weight});
        }

        int minPrice = runDijkstra(adj, years);

        std::cout << minPrice << " ";
    }

    std::cout << "\n";
}