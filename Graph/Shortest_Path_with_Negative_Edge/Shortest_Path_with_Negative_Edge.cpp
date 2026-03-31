#include <iostream>
#include <vector>

struct Edge
{
    int from, to, weight;
};

const int INF = 1e9;

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int nodeCount, edgeCount, start;
    std::cin >> nodeCount >> edgeCount >> start;

    std::vector<Edge> edges(edgeCount);
    for (int i = 0; i < edgeCount; ++i)
    {
        std::cin >> edges[i].from >> edges[i].to >> edges[i].weight;
    }

    std::vector<int> distanceCycle(nodeCount, 0);
    bool hasNegativeCycle = false;

    for (int i = 0; i < nodeCount - 1; ++i)
    {
        for (const auto & edge : edges)
        {
            if (distanceCycle[edge.from] + edge.weight < distanceCycle[edge.to])
            {
                distanceCycle[edge.to] = distanceCycle[edge.from] + edge.weight;
            }
        }
    }

    for (const auto & edge : edges)
    {
        if (distanceCycle[edge.from] + edge.weight < distanceCycle[edge.to])
        {
            hasNegativeCycle = true;
            break;
        }
    }

    if (hasNegativeCycle)
    {
        std::cout << -1;
        return 0;
    }

    std::vector<int> distance(nodeCount, INF);
    distance[start] = 0;

    for (int i = 0; i < nodeCount - 1; ++i)
    {
        for (const auto & edge : edges)
        {
            if (distance[edge.from] != INF && distance[edge.from] + edge.weight < distance[edge.to])
            {
                distance[edge.to] = distance[edge.from] + edge.weight;
            }
        }
    }

    for (int answer : distance)
    {
        std::cout << answer << " ";
    }
}