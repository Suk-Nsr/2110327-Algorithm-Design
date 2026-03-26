#include <iostream>
#include <vector>

bool DFS(int verticeTocheck, std::vector<std::vector<int>> & adjacent, std::vector<bool> & visited, int parent)
{
    visited[verticeTocheck] = true;

    for (int adj : adjacent[verticeTocheck])
    {
        if (visited[adj] == false)
        {
            if (DFS(adj, adjacent, visited, verticeTocheck))
            {
                return true;
            }
        }
        else if (adj != parent)
        {
            return true;
        }
    }
    return false;
}

int main()
{
    int graphs;
    std::cin >> graphs;

    for (int i = 0; i < graphs; ++i)
    {
        int vertices, edges;
        std::cin >> vertices >> edges;

        std::vector<std::vector<int>> adjacent(vertices, std::vector<int>());

        for (int j = 0; j < edges; ++j)
        {
            int vertice1, vertice2;
            std::cin >> vertice1 >> vertice2;
            adjacent[vertice1].push_back(vertice2);
            adjacent[vertice2].push_back(vertice1);
        }

        std::vector<bool> visited(vertices, false);
        bool founded = false;

        for (int j = 0; j < vertices; ++j)
        {
            if (visited[j] == false)
            {
                if (DFS(j, adjacent, visited, -1))
                {
                    std::cout << "YES\n";
                    founded = true;
                    break;
                }
            }
        }

        if (!founded)
        {
            std::cout << "NO\n";
        }
    }
}