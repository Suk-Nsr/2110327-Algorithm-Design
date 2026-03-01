#include <algorithm>
#include <iostream>
#include <vector>

int modulo(int number)
{
    return ((number % 32717) + 32717) % 32717;
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    int k, n;
    std::cin >> k >> n;

    std::vector<int> cValue(k + 1);

    for (int i = 1; i <= k; ++i)
    {
        int c;
        std::cin >> c;
        cValue[i] = modulo(c);
    }

    std::vector<int> aValue(k);

    for (int i = 0; i < k; ++i)
    {
        int a;
        std::cin >> a;
        aValue[i] = modulo(a);
    }

    std::vector<int> rValue(std::max(k, n + 1));

    for (int i = 0; i < k; ++i)
    {
        rValue[i] = aValue[i];
    }

    for (int i = k; i <= n; ++i)
    {
        int currentSum = 0;

        for (int j = 1; j <= k; ++j)
        {
            int term = modulo(cValue[j] * rValue[i - j]);
            currentSum = modulo(currentSum + term);
        }

        rValue[i] = currentSum;
    }

    std::cout << rValue[n];
}