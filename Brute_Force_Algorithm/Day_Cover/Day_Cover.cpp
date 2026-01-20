#include <iostream>
#include <vector>

int minStudents = 20;
std::vector<int> daySlots, studentSlots;
std::vector<std::vector<int>> students;

void permutation(int totalDays, int populations, int dayCount, int studentCount, int from)
{
    if (studentCount >= minStudents)
    {
        return;
    }

    if (dayCount >= totalDays)
    {
        minStudents = std::min(minStudents, studentCount);
        return;
    }

    for (int i = from; i < populations; ++i)
    {
        if (studentSlots[i] == 0)
        {
            studentSlots[i] = 1;
            ++studentCount;

            std::vector<int> markedDays;
            for (int j : students[i])
            {
                if (daySlots[j - 1] == 0)
                {
                    markedDays.push_back(j - 1);
                    ++dayCount;
                    ++daySlots[j - 1];
                }
            }

            permutation(totalDays, populations, dayCount, studentCount, i + 1);

            studentSlots[i] = 0;
            --studentCount;

            for (int k : markedDays)
            {
                --dayCount;
                --daySlots[k];
            }
        }
    }
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int totalDays, populations;
    std::cin >> totalDays >> populations;
    daySlots.resize(totalDays, 0);
    studentSlots.resize(populations, 0);
    students.resize(populations, std::vector<int>());

    for (int i = 0; i < populations; ++i)
    {
        int availableDays;
        std::cin >> availableDays;
        students[i].resize(availableDays);

        for (int j = 0; j < availableDays; ++j)
        {
            std::cin >> students[i][j];
        }
    }

    permutation(totalDays, populations, 0, 0, 0);

    std::cout << minStudents;
}