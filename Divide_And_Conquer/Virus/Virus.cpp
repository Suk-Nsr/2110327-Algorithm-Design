#include <algorithm>
#include <iostream>
#include <vector>

void geneticInput(std::vector<int> & genetic, int geneticSize)
{
    for (int j = 0; j < geneticSize; ++j)
    {
        std::cin >> genetic[j];
    }
}

bool smallestGeneticCheck(std::vector<int> & genetic)
{
    if (!genetic[0] && genetic[1])
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool DIVOCCheck(std::vector<int> & genetic)
{
    if (genetic.size() == 2)
    {
        return smallestGeneticCheck(genetic);
    }
    else
    {
        int mid = genetic.size() >> 1;

        std::vector<int> firstHalf = std::vector<int>(genetic.begin(), genetic.begin() + mid);
        std::vector<int> secondHalf = std::vector<int>(genetic.begin() + mid, genetic.end());
        std::vector<int> mutant(firstHalf.begin(), firstHalf.end());
        std::reverse(mutant.begin(), mutant.end());

        bool possible_1 = DIVOCCheck(firstHalf) && DIVOCCheck(secondHalf);
        bool possible_2 = DIVOCCheck(mutant) && DIVOCCheck(secondHalf);

        return possible_1 || possible_2;
    }
}

int main()
{
    int geneticCount, powerOfTwo, geneticSize = 1;
    std::cin >> geneticCount >> powerOfTwo;

    geneticSize = geneticSize << powerOfTwo;

    std::vector<int> genetic(geneticSize);

    for (int i = 0; i < geneticCount; ++i)
    {
        geneticInput(genetic, geneticSize);
        
        if (DIVOCCheck(genetic))
        {
            std::cout << "yes\n";
        }
        else
        {
            std::cout << "no\n";
        }
    }
}