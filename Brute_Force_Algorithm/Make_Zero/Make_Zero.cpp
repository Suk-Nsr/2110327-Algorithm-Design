#include <iostream>
#include <queue>

int main()
{
    int x, depth = 0;
    bool allGood = true;
    std::queue<int> BFS;
    std::cin >> x;
    BFS.push(x);

    while (allGood)
    {
        int depthSize;
        depthSize = BFS.size();

        while (depthSize--)
        {
            if (BFS.front() % 16777216 == 0)
            {
                allGood = false;
                break;
            }
            else
            {
                BFS.push(BFS.front() + 1);
                BFS.push(BFS.front() * 2);
                BFS.pop();
            }
        }

        if (allGood)
        {
            ++depth;
        }
    }

    if (x == 0)
    {
        std::cout << 0;
    }
    else
    {
        std::cout << depth;
    }
}