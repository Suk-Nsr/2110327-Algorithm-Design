#include <iostream>
#include <vector>

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int squareMatrixSize, maxDegrees = 0;
    std::cin >> squareMatrixSize;

    std::vector<int> degreesCount(squareMatrixSize, 0);

    for (int i = 0; i < squareMatrixSize; ++i)
    {
        int currentDegrees = 0;

        for (int j = 0; j < squareMatrixSize; ++j)
        {
            int connectedTo;
            std::cin >> connectedTo;

            if (connectedTo == 1)
            {
                ++currentDegrees;
            }
        }

        ++degreesCount[currentDegrees];

        if (currentDegrees > maxDegrees)
        {
            maxDegrees = currentDegrees;
        }
    }

    for (int i = 0; i <= maxDegrees; ++i)
    {
        std::cout << degreesCount[i] << " ";
    }
}