#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

int cycleCheckBFS(int startRow, int startColumn, vector<vector<int>>& roadMap, vector<vector<bool>>& visited)
{
    queue<pair<int, int>> q;
    q.push({startRow, startColumn});
    visited[startRow][startColumn] = true;
    
    int nodeCount = 0;
    bool isCycle = true;
    
    while (!q.empty())
    {
        int r = q.front().first;
        int c = q.front().second;
        q.pop();
        
        nodeCount++;
        
        int currentRoad = roadMap[r][c];
        int validConnections = 0;
        
        if ((currentRoad == 1 || currentRoad == 5 || currentRoad == 6) && c - 1 >= 0)
        {
            int nextRoad = roadMap[r][c - 1];

            if (nextRoad == 1 || nextRoad == 3 || nextRoad == 4)
            {
                validConnections++;

                if (!visited[r][c - 1])
                {
                    visited[r][c - 1] = true;
                    q.push({r, c - 1});
                }
            }
        }
        
        if ((currentRoad == 1 || currentRoad == 3 || currentRoad == 4) && c + 1 < roadMap[0].size())
        {
            int nextRoad = roadMap[r][c + 1];

            if (nextRoad == 1 || nextRoad == 5 || nextRoad == 6)
            {
                validConnections++;

                if (!visited[r][c + 1])
                {
                    visited[r][c + 1] = true;
                    q.push({r, c + 1});
                }
            }
        }

        if ((currentRoad == 2 || currentRoad == 3 || currentRoad == 6) && r - 1 >= 0)
        {
            int nextRoad = roadMap[r - 1][c];

            if (nextRoad == 2 || nextRoad == 4 || nextRoad == 5)
            {
                validConnections++;

                if (!visited[r - 1][c])
                {
                    visited[r - 1][c] = true;
                    q.push({r - 1, c});
                }
            }
        }
        
        if ((currentRoad == 2 || currentRoad == 4 || currentRoad == 5) && r + 1 < roadMap.size())
        {
            int nextRoad = roadMap[r + 1][c];
            if (nextRoad == 2 || nextRoad == 3 || nextRoad == 6)
            {
                validConnections++;

                if (!visited[r + 1][c])
                {
                    visited[r + 1][c] = true;
                    q.push({r + 1, c});
                }
            }
        }
        
        if (validConnections != 2)
        {
            isCycle = false;
        }
    }
    
    if (isCycle && nodeCount > 0)
    {
        return nodeCount;
    }
    
    return 0;
}

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int row, column;
    cin >> row >> column;

    int cycleCount = 0;
    int longestCycle = 0;

    vector<vector<int>> roadMap(row, vector<int>(column));
    vector<vector<bool>> visited(row, vector<bool>(column, false));

    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < column; ++j)
        {
            cin >> roadMap[i][j];
        }
    }

    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < column; ++j)
        {
            if (roadMap[i][j] != 0 && !visited[i][j])
            { 
                
                int currentCycleLength = cycleCheckBFS(i, j, roadMap, visited);
                
                if (currentCycleLength > 0)
                {
                    cycleCount++;
                    longestCycle = max(longestCycle, currentCycleLength);
                }
            }
        }
    }

    if (cycleCount == 0)
    {
        cout << "0 0" << "\n";
    }
    else
    {
        cout << cycleCount << " " << longestCycle << "\n";
    }

    return 0;
}