#include <iostream>
#include <utility>
#include <vector>
#include <queue>

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int rows, columns, range;
    std::cin >> rows >> columns >> range;

    std::vector<std::vector<int>> grid(rows, std::vector<int>(columns));
    std::queue<std::pair<int, int>> queue;

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < columns; ++j)
        {
            int type;
            std::cin >> type;
            
            if (type == 1)
            {
                queue.push(std::make_pair(i, j));
            }

            grid[i][j] = type;
        }
    }

    for (int i = 0; i < range; ++i)
    {
        int iteration = queue.size();

        for (int j = 0; j < iteration; ++j)
        {
            int row = queue.front().first;
            int column = queue.front().second;
            queue.pop();

            if (row - 1 >= 0)
            {
                if (grid[row - 1][column] == 0)
                {
                    grid[row - 1][column] = 2;
                    queue.push(std::make_pair(row - 1, column));
                }
            }

            if (row + 1 < rows)
            {
                if (grid[row + 1][column] == 0)
                {
                    grid[row + 1][column] = 2;
                    queue.push(std::make_pair(row + 1, column));
                }
            }

            if (column - 1 >= 0)
            {
                if (grid[row][column - 1] == 0)
                {
                    grid[row][column - 1] = 2;
                    queue.push(std::make_pair(row, column - 1));
                }
            }

            if (column + 1 < columns)
            {
                if (grid[row][column + 1] == 0)
                {
                    grid[row][column + 1] = 2;
                    queue.push(std::make_pair(row, column + 1));
                }
            }
        }
    }

    for (std::vector bigLand : grid)
    {
        for (int land : bigLand)
        {
            std::cout << land << " ";
        }
        
        std::cout << "\n";
    }
}