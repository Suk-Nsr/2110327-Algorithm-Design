#include <iostream>
#include <vector>
#include <queue>

int main()
{
    int rows, columns, days, infectedCount = 0, currentDay = 0;
    std::cin >> rows >> columns >> days;

    std::vector<std::vector<int>> grid(rows, std::vector<int>(columns));
    std::queue<std::pair<int, int>> queue;

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < columns; ++j)
        {
            int data;
            std::cin >> data;

            if (data == 1)
            {
                ++infectedCount;
                queue.push(std::make_pair(i, j));
            }

            grid[i][j] = data;
        }
    }

    while (currentDay != days)
    {
        int iteration = queue.size();

        for (int i = 0; i < iteration; ++i)
        {
            int row = queue.front().first;
            int column = queue.front().second;
            queue.pop();

            if (row - 1 >= 0)
            {
                if (grid[row - 1][column] == 0)
                {
                    ++infectedCount;
                    grid[row - 1][column] = 1;
                    queue.push(std::make_pair(row - 1, column));
                }
            }

            if (row + 1 < rows)
            {
                if (grid[row + 1][column] == 0)
                {
                    ++infectedCount;
                    grid[row + 1][column] = 1;
                    queue.push(std::make_pair(row + 1, column));
                }
            }

            if (column - 1 >= 0)
            {
                if (grid[row][column - 1] == 0)
                {
                    ++infectedCount;
                    grid[row][column - 1] = 1;
                    queue.push(std::make_pair(row, column - 1));
                }
            }

            if (column + 1 < columns)
            {
                if (grid[row][column + 1] == 0)
                {
                    ++infectedCount;
                    grid[row][column + 1] = 1;
                    queue.push(std::make_pair(row, column + 1));
                }
            }
        }

        ++currentDay;
    }

    std::cout << infectedCount;
}