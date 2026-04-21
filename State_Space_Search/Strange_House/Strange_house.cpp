#include <unordered_map>
#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <queue>
#include <stack>

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    
    int rooms, paths;
    std::cin >> rooms >> paths;

    std::string initial = std::string(rooms, 'R') + std::string(rooms, 'B') + "_";
    std::string final = std::string(rooms, 'B') + std::string(rooms, 'R') + "_";

    std::queue<std::pair<std::string, int>> queue;
    queue.push({initial, rooms * 2});

    std::unordered_map<std::string, std::pair<std::string, int>> states;
    std::vector<std::vector<int>> adj(rooms * 2 + 1, std::vector<int>());

    for (int i = 0; i < paths; ++i)
    {
        int from, to;
        std::cin >> from >> to;
        adj[from - 1].push_back(to - 1);
        adj[to - 1].push_back(from - 1);
    }

    while (!queue.empty())
    {
        if (queue.front().first == final)
        {
            break;
        }

        int emptyPos = queue.front().second;

        for (int & swap : adj[emptyPos])
        {
            std::string state = queue.front().first;
            std::swap(state[swap], state[emptyPos]);
            
            if (states.find(state) == states.end())
            {
                states[state] = {queue.front().first, swap + 1};
                queue.push({state, swap});
            }
        }

        queue.pop();
    }

    std::stack<int> backtrackAnswer;
    std::string backtrack = final;

    while (backtrack != initial && states.find(final) != states.end())
    {
        backtrackAnswer.push(states[backtrack].second);
        backtrack = states[backtrack].first;
    }

    if (backtrackAnswer.empty())
    {
        std::cout << "-1";
    }
    else
    {
        std::cout << backtrackAnswer.size() << "\n";
    }

    while (!backtrackAnswer.empty())
    {
        std::cout << backtrackAnswer.top() << " ";
        backtrackAnswer.pop();
    }
}