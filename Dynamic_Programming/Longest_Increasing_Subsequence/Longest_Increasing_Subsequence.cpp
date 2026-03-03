#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    int dataAmount;
    std::cin >> dataAmount;

    std::vector<int> data(dataAmount);
    for (int i = 0; i < dataAmount; ++i)
    {
        std::cin >> data[i];
    }

    std::vector<int> dataSorted(data);
    std::sort(dataSorted.begin(), dataSorted.end());

    std::vector<std::vector<int>> table(dataAmount + 1, std::vector<int>(dataAmount + 1, 0));
    for (int i = 1; i <= dataAmount; ++i)
    {
        for (int j = 1; j <= dataAmount; ++j)
        {
            if (dataSorted[i - 1] == data[j - 1])
            {
                table[i][j] = table[i - 1][j - 1] + 1;
            }
            else
            {
                table[i][j] = std::max(table[i - 1][j], table[i][j - 1]);
            }
        }
    }

    std::cout << table[dataAmount][dataAmount];
}