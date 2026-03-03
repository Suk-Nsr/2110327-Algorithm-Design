#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::string firstString, secondString;
    std::cin >> firstString >> secondString;
    std::vector<std::vector<int>> table(secondString.length() + 1, std::vector<int>(firstString.length() + 1, 0));

    for (int i = 1; i <= secondString.length(); ++i)
    {
        for (int j = 1; j <= firstString.length(); ++j)
        {
            if (secondString[i - 1] == firstString[j - 1])
            {
                table[i][j] = table[i - 1][j - 1] + 1;
            }
            else
            {
                table[i][j] = std::max(table[i - 1][j], table[i][j - 1]);
            }
        }
    }

    std::cout << table[secondString.length()][firstString.length()];
}