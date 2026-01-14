#include <algorithm>
#include <iostream>
#include <vector>
#include <map>

int main()
{
    int roadLength, monsterAmount, turretAmount, turretRange, totalPower = 0, totalDamage = 0;
    std::cin >> roadLength >> monsterAmount >> turretAmount >> turretRange;
    
    std::vector<int> monsterPositions;
    for (int i = 0; i < monsterAmount; ++i)
    {
        int monsterPosition;
        std::cin >> monsterPosition;
        monsterPositions.push_back(monsterPosition);
    }

    std::map<int, int> monsterPowers;
    for (int i = 0; i < monsterAmount; ++i)
    {
        int monsterPower;
        std::cin >> monsterPower;
        monsterPowers[monsterPositions[i]] += monsterPower;
        totalPower += monsterPower;
    }

    std::vector<int> turretPositions;
    for (int i = 0; i < turretAmount; ++i)
    {
        int turretPosition;
        std::cin >> turretPosition;
        turretPositions.push_back(turretPosition);
    }

    std::sort(turretPositions.begin(), turretPositions.end());
    auto it = monsterPowers.begin();

    for (int Pos : turretPositions)
    {
        while (it != monsterPowers.end() && (it->first < Pos - turretRange || it->second == 0))
        {
            ++it;
        }

        if (it != monsterPowers.end() && it->first <= Pos + turretRange)
        {
            it->second--;
            ++totalDamage;
        }
    }
    
    std::cout << totalPower - totalDamage;
}