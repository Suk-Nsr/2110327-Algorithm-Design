#include <algorithm> 
#include <iostream>
#include <utility>
#include <vector>

int getMaxY(std::vector<std::pair<int, int>> & points)
{
    int maxY = -1;
    for (std::pair<int, int> & point : points)
    {
        if (point.second > maxY)
        {
            maxY = point.second;
        }
    }
    return maxY;
}

std::vector<std::pair<int, int>> pareto(std::vector<std::pair<int, int>> & points, int left, int right)
{
    if (left == right)
    {
        return {points[left]};
    }

    int mid = (left + right) >> 1;

    std::vector<std::pair<int, int>> leftAnswer = pareto(points, left, mid);
    std::vector<std::pair<int, int>> rightAnswer = pareto(points, mid + 1, right);

    std::vector<std::pair<int, int>> result;

    for (std::pair<int, int> & answer : rightAnswer)
    {
        result.push_back(answer);
    }

    int rightMaxY = getMaxY(rightAnswer);

    for (std::pair<int, int> & answer : leftAnswer)
    {
        if (answer.second > rightMaxY)
        {
            result.push_back(answer);
        }
    }

    return result;
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int pointCount;
    std::cin >> pointCount;
    std::vector<std::pair<int, int>> points(pointCount);

    for (int i = 0; i < pointCount; ++i)
    {
        int posX, posY;
        std::cin >> posX >> posY;
        points[i] = std::make_pair(posX, posY);
    }

    std::sort(points.begin(), points.end());

    std::vector<std::pair<int, int>> answer = pareto(points, 0, pointCount - 1);

    std::cout << answer.size();
}