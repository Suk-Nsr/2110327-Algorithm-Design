#include <iostream>
#include <limits.h>
#include <vector>

void findMaxValue(std::vector<int> & array, int startIndex, int endIndex, int & maxValue)
{
    if (startIndex == endIndex)
    {
        if (array[startIndex] > maxValue)
        {
            maxValue = array[startIndex];
        }

        return;
    }

    int midIndex = (startIndex + endIndex) >> 1;
    int frontMaxValue = array[midIndex], backMaxValue = array[midIndex + 1];
    int frontValue = 0, backValue = 0;

    for (int i = midIndex; i >= startIndex; --i)
    {
        frontValue += array[i];

        if (frontValue > frontMaxValue)
        {
            frontMaxValue = frontValue;
        }
    }

    for (int i = midIndex + 1; i <= endIndex; ++i)
    {
        backValue += array[i];

        if (backValue > backMaxValue)
        {
            backMaxValue = backValue;
        }
    }

    int maxValueCombined = frontMaxValue + backMaxValue;

    if (maxValueCombined > maxValue)
    {
        maxValue = maxValueCombined;
    }
    
    findMaxValue(array, startIndex, midIndex, maxValue);
    findMaxValue(array, midIndex + 1, endIndex, maxValue);
}

int main()
{
    int arraySize, maxValue = INT_MIN;
    std::cin >> arraySize;
    std::vector<int> array(arraySize);

    for (int i = 0; i < arraySize; ++i)
    {
        std::cin >> array[i];
    }

    findMaxValue(array, 0, arraySize - 1, maxValue);

    std::cout << maxValue;
}