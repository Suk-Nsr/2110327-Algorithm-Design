#include <iostream>
#include <vector>

int main()
{
    int n, m, maxValue = 0;
    std::cin >> n >> m;
    std::vector<int> valueList(n);
    std::vector<int> weightList(n);
    std::vector<std::vector<int>> table(n + 1, std::vector<int>(m + 1));
    std::vector<int> selectedPieces;

    for (int i = 0; i < n; ++i)
    {
        std::cin >> valueList[i];
    }

    for (int i = 0; i < n; ++i)
    {
        std::cin >> weightList[i];
    }

    for (int i = 0; i < n + 1; ++i)
    {
        for (int j = 0; j < m + 1; ++j)
        {
            std::cin >> table[i][j];
        }
    }

    int columnIndex = m;

    for (int i = n; i > 0; --i)
    {
        if (table[i][columnIndex] != table[i - 1][columnIndex])
        {
            selectedPieces.push_back(i);
            columnIndex -= weightList[i - 1];
        }
    }

    std::cout << selectedPieces.size() << "\n";

    for (int piece : selectedPieces)
    {
        std::cout << piece << " ";
    }
}