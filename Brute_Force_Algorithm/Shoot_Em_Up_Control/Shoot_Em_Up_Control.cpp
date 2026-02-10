#include <iostream>
#include <string>
#include <vector>

void solve(int currentTime, int & totalTime, int currentHeight, int & stageHeight, std::string path)
{
    if (currentTime == totalTime)
    {
        std::cout << path << "\n";
        return;
    }

    if (currentHeight - 1 >= 1)
    {
        solve(currentTime + 1, totalTime, currentHeight - 1, stageHeight, path + "D");
    }

    solve(currentTime + 1, totalTime, currentHeight, stageHeight, path + "S");

    if (currentHeight + 1 <= stageHeight)
    {
        solve(currentTime + 1, totalTime, currentHeight + 1, stageHeight, path + "U");
    }
}

int main()
{
    int totalTime, stageHeight, initialHeight;
    std::cin >> totalTime >> stageHeight >> initialHeight;

    solve(0, totalTime, initialHeight, stageHeight, "");
}