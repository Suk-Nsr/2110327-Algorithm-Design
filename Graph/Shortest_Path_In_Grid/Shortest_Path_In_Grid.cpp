#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <queue>

int main()
{
    int rows, columns;
    std::cin >> rows >> columns;

    std::vector<std::string> grid(rows);
    std::vector<std::vector<int>> distance(rows, std::vector<int>(columns, -1));
    distance[0][0] = 0;

    for (int i = 0; i < rows; ++i)
    {
        std::cin >> grid[i];
    }

    std::queue<std::pair<int, int>> queue;
    queue.push(std::make_pair(0,0));

    while (!(queue.empty()))
    {
        int row = queue.front().first;
        int column = queue.front().second;
        queue.pop();

        if (row - 1 >= 0)
        {
            if (grid[row - 1][column] == '.' && distance[row - 1][column] == -1)
            {
                distance[row - 1][column] = distance[row][column] + 1;
                queue.push(std::make_pair(row - 1, column));
            }
        }

        if (row + 1 < rows)
        {
            if (grid[row + 1][column] == '.' && distance[row + 1][column] == -1)
            {
                distance[row + 1][column] = distance[row][column] + 1;
                queue.push(std::make_pair(row + 1, column));
            }
        }

        if (column - 1 >= 0)
        {
            if (grid[row][column - 1] == '.' && distance[row][column - 1] == -1)
            {
                distance[row][column - 1] = distance[row][column] + 1;
                queue.push(std::make_pair(row, column - 1));
            }
        }

        if (column + 1 < columns)
        {
            if (grid[row][column + 1] == '.' && distance[row][column + 1] == -1)
            {
                distance[row][column + 1] = distance[row][column] + 1;
                queue.push(std::make_pair(row, column + 1));
            }
        }
    }

    if (distance[rows - 1][columns - 1] == -1)
    {
        std::cout << -1;
    }
    else
    {
        std::cout << distance[rows - 1][columns - 1];
    }
}