#include <iostream>
#include <vector>
#include <queue>

int main()
{
    int plainLength, windAmount, Fires, sum = 0;
    std::cin >> plainLength >> windAmount >> Fires;

    std::vector<int> plain(plainLength);
    std::vector<std::vector<int>> wind(plainLength, std::vector<int>());
    std::vector<int> startFire(Fires);
    
    for (int i = 0; i < plainLength; ++i)
    {
        int beautiful;
        std::cin >> beautiful;
        sum += beautiful;
        plain[i] = beautiful;
    }
    
    for (int i = 0; i < Fires; ++i)
    {
        std::cin >> startFire[i];
    }
    
    for (int i = 0; i < windAmount; ++i)
    {
        int from, to;
        std::cin >> from >> to;
        wind[from].push_back(to);
    }
    
    for (int i = 0; i < Fires; ++i)
    {
        int startNode = startFire[i];
        
        std::queue<int> queue;

        if (plain[startNode] != 0)
        {
            sum -= plain[startNode];
            plain[startNode] = 0;
            queue.push(startNode);

            while (!queue.empty())
            {
                int curr = queue.front();
                queue.pop();

                for (int destination : wind[curr])
                {
                    if (plain[destination] != 0)
                    {
                        sum -= plain[destination];
                        plain[destination] = 0;
                        queue.push(destination);
                    }
                }
            }
        }

        std::cout << sum << " ";
    }
}