#include <iostream>
#include <vector>
#include <queue>

const int INF = 1e9;

struct Node
{
    int cost, id;

    bool operator>(const Node & other) const
    {
        return cost > other.cost;
    }
};

int main()
{
    int N;
    std::cin >> N;

    std::vector<std::vector<int>> grid(N, std::vector<int>(N));

    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            std::cin >> grid[i][j];
        }
    }

    std::vector<int> distance(N, INF);
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

    distance[0] = 0;
    pq.push({0, 0});

    while (!pq.empty())
    {
        Node current = pq.top();
        pq.pop();

        int u = current.id;
        int current_cost = current.cost;

        if (current_cost > distance[u]) continue;

        for (int v = 0; v < N; ++v)
        {
            int weight = grid[u][v];

            if (weight != -1 && weight != 0)
            {
                int next_cost = current_cost + weight;

                if (next_cost < distance[v])
                {
                    distance[v] = next_cost;
                    pq.push({next_cost, v});
                }
            }
        }
    }

    int max_dist = 0;
    for (int i = 0; i < N; ++i)
    {
        if (distance[i] == INF)
        {
            max_dist = -1;
            break;
        }
        if (distance[i] > max_dist)
        {
            max_dist = distance[i];
        }
    }

    std::cout << max_dist;
}