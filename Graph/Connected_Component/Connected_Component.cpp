#include <iostream>
#include <vector>

void DFS(int node, std::vector<std::vector<int>> & adjacent, std::vector<bool> & visited)
{
    visited[node] = true;

    for (int neighbor : adjacent[node])
    {
        if (visited[neighbor] == false)
        {
            DFS(neighbor, adjacent, visited);
        }
    }
}

int main()
{
    int nodes, edges, componentCount = 0;
    std::cin >> nodes >> edges;

    if (edges == 0)
    {
        std::cout << nodes;
        return 0;
    }

    std::vector<std::vector<int>> adjacent(nodes, std::vector<int>());
    std::vector<bool> visited(nodes, false);

    for (int i = 0; i < edges; ++i)
    {
        int edge1, edge2;
        std::cin >> edge1 >> edge2;
        adjacent[edge1 - 1].push_back(edge2 - 1);
        adjacent[edge2 - 1].push_back(edge1 - 1);
    }

    for (int i = 0; i < nodes; ++i)
    {
        if (visited[i] == false)
        {
            ++componentCount;
            DFS(i, adjacent, visited);
        }
    }

    std::cout << componentCount;
}