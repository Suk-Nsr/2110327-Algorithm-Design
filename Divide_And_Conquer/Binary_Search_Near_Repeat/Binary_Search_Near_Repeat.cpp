#include <iostream>
#include <vector>

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int numberOfElements, numberOfQueries;
    std::cin >> numberOfElements >> numberOfQueries;
    std::vector<int> elements(numberOfElements);

    for (int i = 0; i < numberOfElements; ++i)
    {
        std::cin >> elements[i];
    }

    for (int i = 0; i < numberOfQueries; ++i)
    {
        int query, first = 0, last = elements.size() - 1, mid = last >> 1;
        std::cin >> query;

        if (query < elements[0])
        {
            std::cout << "-1\n";
            continue;
        }

        if (query >= elements[numberOfElements - 1])
        {
            std::cout << numberOfElements - 1 << "\n";
            continue;
        }
        
        while (first != last)
        {
            if (query > elements[mid])
            {
                first = mid;
                ++first;
                mid = (last + first) >> 1;
            }

            if (query < elements[mid])
            {
                last = mid;
                --last;
                mid = last >> 1;
            }
            
            if (query == elements[mid])
            {
                break;
            }
        }

        while (elements[mid + 1] <= query)
        {
            ++mid;
        }

        std::cout << mid << "\n";
    }
}