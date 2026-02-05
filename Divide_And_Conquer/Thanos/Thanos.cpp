#include <algorithm>
#include <iostream>
#include <vector>

int powCal(std::vector<int> & avengerPos, int destructPow, int destructPowSpecial, int start, int stop)
{
    auto it_start = std::lower_bound(avengerPos.begin(), avengerPos.end(), start);
    auto it_stop = std::upper_bound(avengerPos.begin(), avengerPos.end(), stop);
    
    long long avengerCount = it_stop - it_start;
    long long length = stop - start + 1;
    long long currentDestroyCost;

    if (avengerCount == 0) {
        currentDestroyCost = destructPow;
    } else {
        currentDestroyCost = (long long)destructPowSpecial * avengerCount * length;
    }

    if (start == stop || avengerCount == 0) {
        return currentDestroyCost;
    }

    int mid = start + (stop - start) / 2;
    long long leftPowUsed = powCal(avengerPos, destructPow, destructPowSpecial, start, mid);
    long long rightPowUsed = powCal(avengerPos, destructPow, destructPowSpecial, mid + 1, stop);

    return std::min(currentDestroyCost, leftPowUsed + rightPowUsed);
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int powOfTwo, avengerCount, destructPow, destructPowSpecial, baseLength = 1;
    std::cin >> powOfTwo >> avengerCount >> destructPow >> destructPowSpecial;

    baseLength = baseLength << powOfTwo;

    std::vector<int> avengerPos(avengerCount);

    for (int i = 0; i < avengerCount; ++i)
    {
        std::cin >> avengerPos[i];
    }

    std::sort(avengerPos.begin(), avengerPos.end());
    
    std::cout << powCal(avengerPos, destructPow, destructPowSpecial, 1, baseLength);
}