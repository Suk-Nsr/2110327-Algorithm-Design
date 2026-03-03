#include <iostream>
#include <vector>

int main()
{
    int roadLength, constraintLength;
    std::cin >> roadLength >> constraintLength;

    std::vector<int> customerSee(roadLength);
    for (int i = 0; i < roadLength; ++i)
    {
        std::cin >> customerSee[i];
    }

    std::vector<int> constraintList(constraintLength);
    {
        std::cin >> constraintList[i];
    }

    
}