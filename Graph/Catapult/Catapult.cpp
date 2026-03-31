#include <algorithm>
#include <iostream>
#include <vector>
#include <stack>

int timer = 0;

void findSCC(int node, std::vector<std::vector<int>> & adjacents, std::vector<int> & discoverTime, std::vector<int> & lowLinkValue, std::vector<bool> & inStack, std::vector<int> & sccSize, std::stack<int> & stack)
{
    discoverTime[node] = lowLinkValue[node] = ++timer;
    stack.push(node);
    inStack[node] = true;

    for (int neighbor : adjacents[node])
    {
        if (!discoverTime[neighbor])
        {
            findSCC(neighbor, adjacents, discoverTime, lowLinkValue, inStack, sccSize, stack);
            lowLinkValue[node] = std::min(lowLinkValue[node], lowLinkValue[neighbor]);
        }
        else if (inStack[neighbor])
        {
            lowLinkValue[node] = std::min(lowLinkValue[node], discoverTime[neighbor]);
        }
    }

    if (discoverTime[node] == lowLinkValue[node])
    {
        int count = 0;
        
        while (true)
        {
            int N = stack.top();
            stack.pop();
            inStack[N] = false;
            ++count;

            if (node == N)
            {
                break;
            }
        }

        sccSize.push_back(count);
    }
}

int main()
{
    int nodes;
    std::cin >> nodes;

    std::vector<std::vector<int>> adjacents(nodes, std::vector<int>());
    std::vector<int> discoverTime(nodes, 0);
    std::vector<int> lowLinkValue(nodes, 0);
    std::vector<bool> inStack(nodes, false);
    std::vector<int> sccSize;
    std::stack<int> stack;

    for (int i = 0; i < nodes; ++i)
    {
        int catapults;
        std::cin >> catapults;

        for (int j = 0; j < catapults; ++j)
        {
            int to;
            std::cin >> to;
            adjacents[i].push_back(to);
        }
    }

    for (int i = 0; i < nodes; ++i)
    {
        if (!discoverTime[i])
        {
            findSCC(i, adjacents, discoverTime, lowLinkValue, inStack, sccSize, stack);
        }
    }

    std::sort(sccSize.begin(), sccSize.end());

    for (int answer : sccSize)
    {
        std::cout << answer << " ";
    }
}