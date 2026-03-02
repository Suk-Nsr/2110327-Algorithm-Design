#include <algorithm>
#include <iostream>
#include <vector>

int futomakiMaxing(int left, int right, std::vector<int> & futomaki, std::vector<std::vector<int>> & memo)
{
    if (right - left == 1)
    {
        return std::max(futomaki[left], futomaki[right]);
    }

    if (memo[left][right] != -1)
    {
        return memo[left][right];
    }

    int cutLeft = std::max(futomaki[left], futomaki[left + 1]) + futomakiMaxing(left + 2, right, futomaki, memo);
    int cutRight = std::max(futomaki[right], futomaki[right - 1]) + futomakiMaxing(left, right - 2, futomaki, memo);
    int cutBoth = std::max(futomaki[left], futomaki[right]) + futomakiMaxing(left + 1, right - 1, futomaki, memo);

    memo[left][right] = std::max({cutLeft, cutRight, cutBoth});

    return std::max({cutLeft, cutRight, cutBoth});
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    int futomakiLength;
    std::cin >> futomakiLength;
    std::vector<int> futomaki(futomakiLength);

    for (int i = 0; i < futomakiLength; ++i)
    {
        std::cin >> futomaki[i];
    }

    std::vector<std::vector<int>> memo(futomakiLength, std::vector<int>(futomakiLength, -1));

    std::cout << futomakiMaxing(0, futomakiLength - 1, futomaki, memo);
}