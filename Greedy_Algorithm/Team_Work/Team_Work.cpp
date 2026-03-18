#include <algorithm>
#include <iostream>
#include <iomanip>
#include <vector>

int main()
{
    int student, subTasks;
    double totalSum = 0;
    std::cin >> student >> subTasks;

    std::vector<std::vector<int>> students(student, std::vector<int>());
    std::vector<int> tasks(subTasks);

    for (int i = 0; i < subTasks; ++i)
    {
        std::cin >> tasks[i];
    }

    std::sort(tasks.begin(), tasks.end());

    for (int i = 0; i < subTasks; ++i)
    {
        students[i % student].push_back(tasks[i]);
    }

    for (int i = 0; i < student; ++i)
    {
        for (int j = students[i].size() - 1; j >= 0; --j)
        {
            totalSum += (students[i].size() - j) * students[i][j];
        }
    }

    totalSum /= subTasks;

    std::cout << std::fixed << std::setprecision(3) << totalSum;
}