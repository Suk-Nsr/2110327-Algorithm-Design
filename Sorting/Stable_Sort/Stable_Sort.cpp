#include <iostream>
#include <vector>

int main()
{
    int dataAmount, answerNeeded;
    std::cin >> dataAmount >> answerNeeded;

    std::vector<int> data(dataAmount);
    for (int i = 0; i < dataAmount; ++i)
    {
        std::cin >> data[i];
    }

    for (int i = 0; i < answerNeeded; ++i)
    {
        std::vector<int> check(dataAmount);
        bool sorted = true, stable = true;
        for (int j = 0; j < dataAmount; ++j)
        {
            std::cin >> check[j];
            if (j > 0)
            {
                if (data[check[j] - 1] < data[check[j - 1] - 1])
                {
                    sorted = false;
                }
                if (data[check[j] - 1] == data[check[j - 1] - 1])
                {
                    if (check[j] - 1 < check[j - 1] - 1)
                    {
                        stable = false;
                    }
                }
            }
        }
        
        if (sorted)
        {
            std::cout << "1 ";
        }
        else
        {
            std::cout << "0 ";
        }

        if (!sorted)
        {
            std::cout << "0\n";
        }
        else
        {
            if (stable)
            {
                std::cout << "1\n";
            }
            else
            {
                std::cout << "0\n";
            }
        }
    }
}