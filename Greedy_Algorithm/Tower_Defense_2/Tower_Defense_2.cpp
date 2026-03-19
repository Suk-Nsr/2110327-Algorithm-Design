#include <algorithm>
#include <iostream>
#include <vector>

struct monsterPosnPow
{
    int position;
    int power;
};

bool comparePosition(monsterPosnPow & a, monsterPosnPow & b)
{
    return a.position < b.position;
}

int main()
{
    int pathLength, monsterAmount, turretAmount, turretRange, mostRight = 1, powerSum = 0, currentTurret = 0;
    std::cin >> pathLength >> monsterAmount >> turretAmount >> turretRange;

    std::vector<monsterPosnPow> monsters(monsterAmount);

    for (int i = 0; i < monsterAmount; ++i)
    {
        std::cin >> monsters[i].position;
    }

    for (int i = 0; i < monsterAmount; ++i)
    {
        std::cin >> monsters[i].power;
        powerSum += monsters[i].power;
    }

    std::sort(monsters.begin(), monsters.end(), comparePosition);

    for (int i = 0; i < monsterAmount; ++i)
    {
        int possibleLeft = std::max(monsters[i].position - turretRange, 1);
        int possibleRight = std::min(monsters[i].position + turretRange, pathLength);

        while (mostRight <= possibleRight && monsters[i].power != 0 && currentTurret != turretAmount)
        {
            if (mostRight >= possibleLeft && mostRight <= possibleRight)
            {
                --monsters[i].power;
                --powerSum;
                ++mostRight;
                ++currentTurret;
                continue;
            }
            if (mostRight < possibleLeft)
            {
                mostRight = possibleLeft;
            }
        }
    }

    std::cout << powerSum;
}