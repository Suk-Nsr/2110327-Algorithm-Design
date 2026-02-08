#include <iostream>
#include <vector>

int getOnes(int digits)
{
    int oneCounts = 0;

    for (int i = 0; i < digits; ++i)
    {
        oneCounts = oneCounts * 10 + 1;
    }

    return oneCounts;
}

int solve(int number, int digits)
{
    if (number == 0)
    {
        return 0;
    }

    if (digits == 1)
    {
        return number;
    }

    int repunit = getOnes(digits);

    int plusBy = number / repunit;
    int leftOver = number % repunit;

    int cost1 = plusBy * digits + solve(leftOver, digits - 1);
    int cost2 = (plusBy + 1) * digits + solve(repunit - leftOver, digits - 1);

    return std::min(cost1, cost2);
}

int main()
{
    int number;
    std::cin >> number;

    int digits = 0;
    int temp = number;

    while (temp > 0)
    {
        temp /= 10;
        ++digits;
    }
    
    std::cout << solve(number, digits + 1);
}