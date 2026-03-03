#include <iostream>
#include <vector>

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    int n, aAmount;
    std::cin >> n >> aAmount;
    std::vector<int> rRelation(n + 1, 0);

    std::vector<int> aValue(aAmount);
    for (int i = 0; i < aAmount; ++i)
    {
        std::cin >> aValue[i];
    }

    std::vector<std::vector<int>> b_ij;
    for (int i = 0; i < n - aAmount; ++i)
    {
        int vectorSize;
        std::cin >> vectorSize;
        std::vector<int> subVector(vectorSize);
        for (int j = 0; j < vectorSize; ++j)
        {
            std::cin >> subVector[j];
        }
        b_ij.push_back(subVector);
    }

    for (int i = 1; i <= n; ++i)
    {
        if (i <= 0)
        {
            rRelation[i] = 0;
            continue;
        }

        if (0 < i && i <= aAmount)
        {
            rRelation[i] = (aValue[i - 1]) % 32717;
        }

        if (aAmount < i)
        {
            long long value = 0;

            for (int j = 0; j < b_ij[i - aAmount - 1].size(); ++j)
            {
                int index = i - b_ij[i - aAmount - 1][j];

                if (index >= 0)
                {
                    value = value + rRelation[index];
                }
            }
            rRelation[i] = value % 32717;
        }
    }

    for (int i = 1; i <= n; ++i)
    {
        std::cout << (rRelation[i]) % 32717 << " ";
    }
}