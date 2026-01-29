#include <iostream>
#include <vector>

int InversionCount = 0;

std::vector<int> merge(std::vector<int> & firstHalf, std::vector<int> & secondHalf)
{
    int mergedIndex = 0, firstHalfIndex = 0, secondHalfIndex = 0;
    std::vector<int> merged(firstHalf.size() + secondHalf.size());

    while (firstHalfIndex < firstHalf.size() && secondHalfIndex < secondHalf.size())
    {
        if (firstHalf[firstHalfIndex] <= secondHalf[secondHalfIndex])
        {
            merged[mergedIndex++] = firstHalf[firstHalfIndex++];
        }
        else
        {
            merged[mergedIndex++] = secondHalf[secondHalfIndex++];
            InversionCount += firstHalf.size() - firstHalfIndex;
        }   
    }

    while (firstHalfIndex < firstHalf.size())
    {
        merged[mergedIndex++] = firstHalf[firstHalfIndex++];
    }

    while (secondHalfIndex < secondHalf.size())
    {
        merged[mergedIndex++] = secondHalf[secondHalfIndex++];
    }

    return merged;
}

std::vector<int> mergeSort(std::vector<int> & elements, int startIndex, int endIndex)
{
    if (startIndex == endIndex)
    {
        return {elements[startIndex]};
    }

    std::vector<int> firstHalf = mergeSort(elements, startIndex, (startIndex + endIndex) >> 1);
    std::vector<int> secondHalf = mergeSort(elements, (startIndex + endIndex >> 1) + 1, endIndex);
    std::vector<int> merged = merge(firstHalf, secondHalf);

    return merged;
}

int main()
{
    int arraySizes;
    std::cin >> arraySizes;
    std::vector<int> elements(arraySizes);

    for (int i = 0; i < arraySizes; ++i)
    {
        std::cin >> elements[i];
    }

    std::vector<int> sortedElements = mergeSort(elements, 0, arraySizes - 1);

    std::cout << InversionCount;
}