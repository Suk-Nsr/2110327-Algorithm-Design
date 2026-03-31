#include <iostream>
#include <utility>
#include <vector>
#include <queue>

int main()
{
    int rows, columns, worms, northCount = 1, southmaxCount = 0;
    std::cin >> rows >> columns >> worms;

    std::vector<std::vector<int>> northLand(rows, std::vector<int>(columns));
    std::vector<std::vector<bool>> northVisited(rows, std::vector<bool>(columns, false));
    std::vector<std::vector<int>> southLand(rows, std::vector<int>(columns));
    std::vector<std::vector<bool>> southVisited(rows, std::vector<bool>(columns, false));
    std::vector<std::pair<int, int>> wormPos;

    std::queue<std::pair<int, int>> northQueue;
    std::queue<std::pair<int, int>> southQueue;
    northQueue.push(std::make_pair(0,0));

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < columns; ++j)
        {
            std::cin >> northLand[i][j];
        }
    }

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < columns; ++j)
        {
            std::cin >> southLand[i][j];
        }
    }

    for (int i = 0; i < worms; ++i)
    {
        int row, column;
        std::cin >> row >> column;
        wormPos.push_back(std::make_pair(row - 1, column - 1));
    }

    while (!northQueue.empty())
    {
        int row = northQueue.front().first;
        int column = northQueue.front().second;
        northQueue.pop();
        northVisited[row][column] = true;

        if (row - 1 >= 0)
        {
            if (!northVisited[row - 1][column] && northLand[row - 1][column] == 0)
            {
                northVisited[row - 1][column] = true;
                northQueue.push(std::make_pair(row - 1, column));
                ++northCount;
            }
        }

        if (row + 1 < rows)
        {
            if (!northVisited[row + 1][column] && northLand[row + 1][column] == 0)
            {
                northVisited[row + 1][column] = true;
                northQueue.push(std::make_pair(row + 1, column));
                ++northCount;
            }
        }

        if (column - 1 >= 0)
        {
            if (!northVisited[row][column - 1] && northLand[row][column - 1] == 0)
            {
                northVisited[row][column - 1] = true;
                northQueue.push(std::make_pair(row, column - 1));
                ++northCount;
            }
        }

        if (column + 1 < columns)
        {
            if (!northVisited[row][column + 1] && northLand[row][column + 1] == 0)
            {
                northVisited[row][column + 1] = true;
                northQueue.push(std::make_pair(row, column + 1));
                ++northCount;
            }
        }
    }

    for (std::pair<int, int> Pos : wormPos)
    {
        if (northVisited[Pos.first][Pos.second])
        {
            southQueue.push(std::make_pair(Pos.first, Pos.second));
            int southCount = 1;

            while (!southQueue.empty())
            {
                int row = southQueue.front().first;
                int column = southQueue.front().second;
                southQueue.pop();
                southVisited[row][column] = true;

                if (row - 1 >= 0)
                {
                    if (!southVisited[row - 1][column] && southLand[row - 1][column] == 0)
                    {
                        southVisited[row - 1][column] = true;
                        southQueue.push(std::make_pair(row - 1, column));
                        ++southCount;
                    }
                }

                if (row + 1 < rows)
                {
                    if (!southVisited[row + 1][column] && southLand[row + 1][column] == 0)
                    {
                        southVisited[row + 1][column] = true;
                        southQueue.push(std::make_pair(row + 1, column));
                        ++southCount;
                    }
                }

                if (column - 1 >= 0)
                {
                    if (!southVisited[row][column - 1] && southLand[row][column - 1] == 0)
                    {
                        southVisited[row][column - 1] = true;
                        southQueue.push(std::make_pair(row, column - 1));
                        ++southCount;
                    }
                }

                if (column + 1 < columns)
                {
                    if (!southVisited[row][column + 1] && southLand[row][column + 1] == 0)
                    {
                        southVisited[row][column + 1] = true;
                        southQueue.push(std::make_pair(row, column + 1));
                        ++southCount;
                    }
                }
            }
            southmaxCount = std::max(southmaxCount, southCount);
        }
    }
    std::cout << northCount + southmaxCount;
}