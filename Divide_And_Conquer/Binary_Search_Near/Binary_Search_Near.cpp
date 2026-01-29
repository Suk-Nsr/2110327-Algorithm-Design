#include <iostream>
#include <vector>

std::vector<int> elements;
int answerIndex;

void binarySearch(int startPos, int endPos, int query)
{
    if (startPos > endPos)
    {
        return;
    }

    int midPos = (startPos + endPos) >> 1;

    if (elements[midPos] <= query)
    {
        answerIndex = midPos;
        binarySearch(midPos + 1, endPos, query);
    }
    else
    {
        binarySearch(startPos, midPos - 1, query);
    }
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int numberOfElements, numberOfQueries;
    std::cin >> numberOfElements >> numberOfQueries;
    elements.assign(numberOfElements, 0);

    for (int i = 0; i < numberOfElements; ++i)
    {
        std::cin >> elements[i];
    }

    for (int i = 0; i < numberOfQueries; ++i)
    {
        int query;
        std::cin >> query;

        if (query < elements[0])
        {
            std::cout << "-1\n";
            continue;
        }

        if (query >= elements[elements.size() - 1])
        {
            std::cout << elements[elements.size() - 1] << "\n";
            continue;
        }

        binarySearch(0, numberOfElements - 1, query);

        std::cout << elements[answerIndex] << "\n";
    }
}