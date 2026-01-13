#include <iostream>
#include <vector>

void recur(int available, int currentSize, int maxedSize, std::vector<int> Answer)
{
    if (currentSize < maxedSize)
    {
        Answer[currentSize] = 0;
        recur(available, currentSize + 1, maxedSize, Answer);
        if (available > 0)
        {
            Answer[currentSize] = 1;
            recur(available - 1, currentSize + 1, maxedSize, Answer);
        }
    }
    else
    {
        if (available == 0)
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
    int picked, size;
    std::cin >> picked >> size;
    std::vector<int> Answer(size);
    recur(picked, 0, size, Answer);
}