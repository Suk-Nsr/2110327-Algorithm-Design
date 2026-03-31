#include <iostream>
#include <vector>
#include <queue>

const int INF = 1e9;

struct Node
{
    int cost, row, column;

    bool operator>(const Node & other) const
    {
        return cost > other.cost;
    }
};

int main()
{
    int rows, columns;
    std::cin >> rows >> columns;

    std::vector<std::vector<int>> grid(rows, std::vector<int>(columns));

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < columns; ++j)
        {
            std::cin >> grid[i][j];
        }
    }

    std::vector<std::vector<int>> distance(rows, std::vector<int>(columns, INF));

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

    distance[0][0] = 0;
    pq.push({0, 0, 0});

    int dr[] = {0, 0, 1, -1};
    int dc[] = {1, -1, 0, 0};

    while (!pq.empty())
    {
        Node current = pq.top();
        pq.pop();

        int current_cost = current.cost;
        int r = current.row;
        int c = current.column;

        if (current_cost > distance[r][c]) continue;

        for (int i = 0; i < 4; ++i)
        {
            int nr = r + dr[i];
            int nc = c + dc[i];

            if (nr >= 0 && nr < rows && nc >= 0 && nc < columns)
            {
                int next_cost = current_cost + grid[nr][nc];

                if (next_cost < distance[nr][nc])
                {
                    distance[nr][nc] = next_cost;
                    pq.push({next_cost, nr, nc});
                }
            }
        }
    }

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < columns; ++j)
        {
            std::cout << distance[i][j] << " ";
        }

        std::cout << "\n";
    }
}