#include <iostream>
#include <vector>

void geneticInput(std::vector<int> & genetic, int geneticLength)
{
    for (int i = 0; i < geneticLength; ++i)
    {
        std::cin >> genetic[i];
    }
}

int DIVOCCheck(std::vector<int> & genetic, int start, int stop, int size, bool & check)
{
    if (size == 2)
    {
        int oneCount = 0;

        for (int i = start; i <= stop; ++i)
        {
            if (genetic[i] == 1)
            {
                ++oneCount;
            }
        }

        return oneCount;
    }
    else
    {
        int mid = (start + stop) >> 1;

        int Left = DIVOCCheck(genetic, start, mid, size / 2, check);
        int Right = DIVOCCheck(genetic, mid + 1, stop, size / 2, check);

        if (Left - Right > 1 || Right - Left > 1)
        {
            check = false;
            return 0;
        }
        else
        {
            return Left + Right;
        }
    }
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int geneticCount, powerOfTwo, geneticLength = 1;
    std::cin >> geneticCount >> powerOfTwo;

    if (powerOfTwo == 1)
    {
        for (int i = 0; i < geneticCount; ++i)
        {
            std::cout << "yes\n";
        }
    }
    else
    {
        for (int i = 0; i < powerOfTwo; ++i)
        {
            geneticLength *= 2;
        }
    }

    std::vector<int> genetic(geneticLength);

    for (int i = 0; i < geneticCount; ++i)
    {
        bool check = true;
        geneticInput(genetic, geneticLength);
        int justCountOfOne = DIVOCCheck(genetic, 0, geneticLength - 1, geneticLength, check);

        if (check)
        {
            std::cout << "yes\n";
        }
        else
        {
            std::cout << "no\n";
        }
    }
}