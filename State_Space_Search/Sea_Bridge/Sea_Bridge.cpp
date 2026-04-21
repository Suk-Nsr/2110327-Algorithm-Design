#include <iostream>
#include <utility>
#include <climits>
#include <vector>
#include <queue>

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int mapRows, mapColumns;
    std::cin >> mapRows >> mapColumns;
    
    std::queue<std::pair<int, int>> queue;
    std::vector<std::vector<int>> map(mapRows, std::vector<int>(mapColumns));
    std::vector<std::vector<int>> distance(mapRows, std::vector<int>(mapColumns, INT_MAX));
    std::vector<std::pair<int, int>> direction({{-1, 0}, {0, 1}, {1, 0}, {0, -1}});

    for (int i = 0; i < mapRows; ++i)
    {
        for (int j = 0; j < mapColumns; ++j)
        {
            std::cin >> map[i][j];
            
            if (map[i][j] == 1)
            {
                queue.push({i, j});
                distance[i][j] = 1;
            }
        }
    }

    while (!queue.empty())
    {
        int currentRow = queue.front().first;
        int currentColumn = queue.front().second;

        if (map[currentRow][currentColumn] == 2)
        {
            break;
        }

        for (int i = 0; i < 4; ++i)
        {
            int directionRow = direction[i].first;
            int directionColumn = direction[i].second;

            int newRow = currentRow + directionRow;
            int newColumn = currentColumn + directionColumn;

            if (newRow >= 0 && newRow < mapRows && newColumn >= 0 && newColumn < mapColumns && map[newRow][newColumn] != 3)
            {
                if (distance[newRow][newColumn] == INT_MAX)
                {
                    distance[newRow][newColumn] = distance[currentRow][currentColumn] + 1;
                    queue.push({newRow, newColumn});
                    continue;
                }

                if (distance[newRow][newColumn] > distance[currentRow][currentColumn] + 1)
                {
                    distance[newRow][newColumn] = distance[currentRow][currentColumn] + 1;
                    queue.push({newRow, newColumn});
                    continue;
                }
            }
        }

        queue.pop();
    }

    std::cout << distance[queue.front().first][queue.front().second];
}