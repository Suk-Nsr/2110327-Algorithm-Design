#include <iostream>

int main()
{
    long long base, power, modulo;
    std::cin >> base >> power >> modulo;
    long long answer = 1;
    base = base % modulo;

    while (power > 0)
    {
        if (power & 1)
        {
            answer = (answer * base) % modulo;
        }
        base = (base * base) % modulo;
        power = power >> 1;
    }

    std::cout << answer;
}