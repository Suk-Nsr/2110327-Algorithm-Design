#include <iostream>
#include <vector>

int DFS(int node, std::vector<std::vector<int>> & adjacents, std::vector<int> & memo, std::vector<int> nodeValues)
{
    if (memo[node] != -1)
    {
        return memo[node];
    }
    else
    {
        int maxValue = nodeValues[node];

        for (int neighbor : adjacents[node])
        {
            maxValue = std::max(maxValue, DFS(neighbor, adjacents, memo, nodeValues));
        }

        memo[node] = maxValue;

        return maxValue;
    }
}

int main()
{
    int nodes, paths, maxDiff = 0;
    std::cin >> nodes >> paths;

    std::vector<int> nodeValues(nodes);
    std::vector<std::vector<int>> adjacents(nodes, std::vector<int>());
    std::vector<int> memo(nodes, -1);

    for (int i = 0; i < nodes; ++i)
    {
        std::cin >> nodeValues[i];
    }

    for (int i = 0; i < paths; ++i)
    {
        int from, to;
        std::cin >> from >> to;
        adjacents[from].push_back(to);
    }

    for (int i = 0; i < nodes; ++i)
    {
        maxDiff = std::max(maxDiff, DFS(i, adjacents, memo, nodeValues) - nodeValues[i]);
    }

    std::cout << maxDiff;
}