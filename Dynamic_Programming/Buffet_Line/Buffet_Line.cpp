#include <iostream>
#include <vector>

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int foodType, personCount, deliciousLoss;
    std::cin >> foodType >> personCount >> deliciousLoss;

    std::vector<int> foodList(foodType + 1, 0);

    for (int i = 1; i <= foodType; ++i)
    {
        int delicious;
        std::cin >> delicious;
        foodList[i] = foodList[i - 1] + (delicious - deliciousLoss);
    }

    for (int i = 0; i < personCount; ++i)
    {
        int startPos, target, realTarget;
        std::cin >> startPos >> target;

        realTarget = foodList[startPos - 1] + target;

        auto it = std::lower_bound(foodList.begin() + 1, foodList.end(), realTarget);

        std::cout << (it - foodList.begin()) << "\n";
    }
}