#include <iostream>

int findSequence(int number)
{
    if (number == 0 || number == 1)
    {
        return number;
    }

    int value;
    int prevNumber_1 = 0;
    int prevNumber_2 = 1;

    for (int i = 2; i <= number; ++i)
    {
        value = prevNumber_1 + prevNumber_2;
        prevNumber_1 = prevNumber_2;
        prevNumber_2 = value;
    }

    return value;
}

int main()
{
    int number;
    std::cin >> number;
    std::cout << findSequence(number);
}