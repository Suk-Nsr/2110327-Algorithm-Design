#include <iostream>
#include <vector>

int cycleLength = 0;

void DFS(int node, int parent, int currentDepth, std::vector<std::vector<int>> & adjacents, std::vector<bool> & visited, std::vector<int> & depth)
{
    visited[node] = true;
    depth[node] = currentDepth;

    for (int neighbor : adjacents[node])
    {
        if (neighbor == parent)
        {
            continue;
        }

        if (visited[neighbor] == true)
        {
            if (cycleLength == 0)
            {
                cycleLength = currentDepth - depth[neighbor] + 1;
            }
        }
        else
        {
            DFS(neighbor, node, currentDepth + 1, adjacents, visited, depth);
        }
    }
}

int main()
{
    int nodes;
    std::cin >> nodes;

    std::vector<std::vector<int>> adjacents(nodes, std::vector<int>());
    std::vector<bool> visited(nodes, false);
    std::vector<int> depth(nodes);

    for (int i = 0; i < nodes; ++i)
    {
        int from, to;
        std::cin >> from >> to;
        adjacents[from].push_back(to);
        adjacents[to].push_back(from);
    }

    DFS(0, -1, 0, adjacents, visited, depth);

    std::cout << cycleLength;
}