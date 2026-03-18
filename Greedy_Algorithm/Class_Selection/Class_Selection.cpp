#include <algorithm>
#include <iostream>
#include <vector>

struct timeTable
{
    int start;
    int stop;
};

bool endTimeCompare(timeTable & a, timeTable & b)
{
    return a.stop < b.stop;
}

int main()
{
    int numberOfClass;
    std::cin >> numberOfClass;

    std::vector<timeTable> classTimeTable(numberOfClass);

    for (int i = 0; i < numberOfClass; ++i)
    {
        std::cin >> classTimeTable[i].start;
    }

    for (int i = 0; i < numberOfClass; ++i)
    {
        std::cin >> classTimeTable[i].stop;
    }

    std::sort(classTimeTable.begin(), classTimeTable.end(), endTimeCompare);

    int useableClass = 0, prevStop = 0;

    for (int i = 0; i < numberOfClass; ++i)
    {
        if (prevStop <= classTimeTable[i].start)
        {
            ++useableClass;
            prevStop = classTimeTable[i].stop;
        }
        else
        {
            continue;
        }
    }

    std::cout << useableClass;
}