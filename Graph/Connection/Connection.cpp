#include <iostream>
#include <vector>
#include <queue>
#include <set>

int main()
{
    int students, relations, degrees, maxFriends = 0;
    std::cin >> students >> relations >> degrees;

    std::vector<std::vector<int>> adjacents(students, std::vector<int>());

    for (int i = 0; i < relations; ++i)
    {
        int from, to;
        std::cin >> from >> to;
        adjacents[from].push_back(to);
        adjacents[to].push_back(from);
    }

    for (int i = 0; i < students; ++i)
    {
        std::queue<int> queue;
        std::set<int> friends;
        queue.push(i);
        friends.insert(i);

        for (int k = 0; k < degrees; ++k)
        {
            int iteration = queue.size();

            for (int j = 0; j < iteration; ++j)
            {
                for (int neighbor : adjacents[queue.front()])
                {
                    if (friends.insert(neighbor).second == true);
                    {
                        queue.push(neighbor);
                    }
                }

                queue.pop();
            }
        }

        maxFriends = std::max(maxFriends, int(friends.size()));
    }

    std::cout << maxFriends;
}