#include <iostream>
#include <vector>

void interruptVector(std::vector<int> & pinnarayaslyra, int startPos, int endPos, int & timeToMergeSort)
{
    if (timeToMergeSort > 0 && startPos != endPos)
    {
        int midPos = (startPos + endPos - 1) >> 1;
        std::swap(pinnarayaslyra[midPos], pinnarayaslyra[midPos + 1]);
        timeToMergeSort -= 2;
        interruptVector(pinnarayaslyra, startPos, midPos, timeToMergeSort);
        interruptVector(pinnarayaslyra, midPos + 1, endPos, timeToMergeSort);
    }
    else
    {
        return;
    }
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int orderCount, timeToMergeSort;
    std::cin >> orderCount >> timeToMergeSort;

    if (timeToMergeSort & 1 && !(timeToMergeSort > 2 * orderCount - 1))
    {
        std::vector<int> pinnarayaslyra(orderCount);
        --timeToMergeSort;
    
        for (int i = 0; i < orderCount; ++i)
        {
            pinnarayaslyra[i] = i + 1;
        }

        interruptVector(pinnarayaslyra, 0, orderCount - 1, timeToMergeSort);

        for (int pinny : pinnarayaslyra)
        {
            std::cout << pinny << " ";
        }
    }
    else
    {
        std::cout << "-1";
    }
}