#include <iostream>
#include <vector>

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int elementInArray, numberOfQueries;
    std::cin >> elementInArray >> numberOfQueries;

    std::vector<int> elements(elementInArray);
    for (int i = 0; i < elementInArray; ++i)
    {
        std::cin >> elements[i];
    }

    for (int i = 0; i < numberOfQueries; ++i)
    {
        bool founded = false;
        int query;
        std::cin >> query;
        for (int lockedNumber = 0; lockedNumber < elements.size() - 3; ++lockedNumber)
        {
            int frontIterator = lockedNumber + 1;
            int backIterator = elements.size() - 1;

            while (frontIterator != backIterator)
            {
                int answer = elements[lockedNumber] + elements[frontIterator] + elements[backIterator];

                if (answer == query)
                {
                    founded = true;
                    break;
                }

                if (answer > query)
                {
                    --backIterator;
                }

                if (answer < query)
                {
                    ++frontIterator;
                }
            }
        }
        if (founded)
        {
            std::cout << "YES" << "\n";
        }
        else
        {
            std::cout << "NO" << "\n";
        }
    }
}