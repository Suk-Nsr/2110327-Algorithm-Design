#include <iostream>
#include <map>

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int pairAmount, questionAmount;
    std::map<int, int> numberAmount;
    std::cin >> pairAmount >> questionAmount;

    for (int i = 0; i < pairAmount; ++i)
    {
        int number, amount;
        std::cin >> number >> amount;
        numberAmount[number] += amount;
    }

    std::map<int, int> indexPos;
    int index = 1;

    for (auto it = numberAmount.begin(); it != numberAmount.end(); ++it)
    {
        indexPos[index] = it->first;
        index += it->second;
    }

    for (int i = 0; i < questionAmount; ++i)
    {
        int searchedIndex;
        std::cin >> searchedIndex;
        auto it = indexPos.upper_bound(searchedIndex);
        if (it != indexPos.begin())
        {
            --it;
        }
        std::cout << it->second << "\n";
    }
}