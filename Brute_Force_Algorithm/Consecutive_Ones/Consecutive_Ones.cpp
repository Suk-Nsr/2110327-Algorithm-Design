#include <iostream>
#include <vector>

void recur(int currentConsecutive, int maxedConsecutive, int currentSize, int maxedSize, int consecutiveNeeded, std::vector<int> & Answer)
{
    if (currentSize < maxedSize)
    {
        Answer[currentSize] = 0;
        recur(0, maxedConsecutive, currentSize + 1, maxedSize, consecutiveNeeded, Answer);
        Answer[currentSize] = 1;
        if (currentConsecutive + 1 > maxedConsecutive)
        {
            maxedConsecutive = currentConsecutive + 1;
        }
        recur(currentConsecutive + 1, maxedConsecutive, currentSize + 1, maxedSize, consecutiveNeeded, Answer);
    }
    else
    {
        if (maxedConsecutive >= consecutiveNeeded)
        {
            for (int & x : Answer)
            {
                std::cout << x;
            }
            std::cout << "\n";
        }
    }
}

int main()
{
    int size, consecutive;
    std::cin >> size >> consecutive;
    std::vector<int> Answer(size);
    recur(0, 0, 0, size, consecutive, Answer);
}