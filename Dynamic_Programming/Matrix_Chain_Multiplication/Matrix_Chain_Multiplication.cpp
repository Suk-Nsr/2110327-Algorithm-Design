#include <iostream>
#include <limits.h>
#include <vector>

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    int matrixCount;
    std::cin >> matrixCount;
    std::vector<int> matrixSize(matrixCount + 1);

    for (int i = 0; i <= matrixCount; ++i)
    {
        std::cin >> matrixSize[i];
    }

    std::vector<std::vector<int>> matrixValue(matrixCount + 1, std::vector<int>(matrixCount + 1, 0));

    for (int i = 2; i <= matrixCount; ++i)
    {
        int matrixCordRow = 1;
        int matrixCordColumn = i;
        while (matrixCordColumn <= matrixCount)
        {
            matrixValue[matrixCordRow][matrixCordColumn] = INT_MAX;

            for (int j = matrixCordRow; j < matrixCordColumn; ++j)
            {
                int currentCost = matrixValue[matrixCordRow][j] + matrixValue[j + 1][matrixCordColumn] + (matrixSize[matrixCordRow - 1] * matrixSize[j] * matrixSize[matrixCordColumn]);

                if (currentCost < matrixValue[matrixCordRow][matrixCordColumn])
                {
                    matrixValue[matrixCordRow][matrixCordColumn] = currentCost;
                }
            }

            ++matrixCordRow;
            ++matrixCordColumn;
        }
    }

    std::cout << matrixValue[1][matrixCount];
}