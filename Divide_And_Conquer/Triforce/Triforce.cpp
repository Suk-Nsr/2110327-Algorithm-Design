#include <iostream>
#include <vector>

std::vector<std::vector<int>> triForceInput()
{
    int size;
    std::cin >> size;
    std::vector<std::vector<int>> triForce(size, std::vector<int>(size));
    
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            std::cin >> triForce[i][j];
        }
    }

    return triForce;
}

bool isAllZero(std::vector<std::vector<int>> checkTriForce, int row_start, int column_start, int row_end, int column_end)
{
    for (int i = row_start; i <= row_end; ++i)
    {
        for (int j = column_start; j <= column_end; ++j)
        {
            if (checkTriForce[i][j] != 0)
            {
                return false;
            }
        }
    }
    return true;
}

bool hasZero(int topLeft, int topRight, int bottomLeft)
{
    if (topLeft == 0 || topRight == 0 || bottomLeft == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int mostFrequentValue(int topLeft, int topRight, int bottomLeft)
{
    if (topLeft == topRight)
    {
        return topLeft;
    }

    if (topRight == bottomLeft)
    {
        return topRight;
    }

    if (bottomLeft == topLeft)
    {
        return bottomLeft;
    }

    return 0;
}

int calculateTriForce(std::vector<std::vector<int>> & checkTriForce, int currentSquareSize, int row_start, int column_start, int row_end, int column_end)
{
    int topLeft, topRight, bottomLeft, bottomRight;

    if (currentSquareSize == 2)
    {
        topLeft = checkTriForce[row_start][column_start];
        topRight = checkTriForce[row_start][column_end];
        bottomLeft = checkTriForce[row_end][column_start];
        bottomRight = checkTriForce[row_end][column_end];

        if (bottomRight != 0)
        {
            return 0;
        }

        if (hasZero(topLeft, topRight, bottomLeft))
        {
            return 0;
        }

        return mostFrequentValue(topLeft, topRight, bottomLeft);
    }
    else
    {
        int row_mid = (row_start + row_end) / 2;
        int column_mid = (column_start + column_end) / 2;

        if (!isAllZero(checkTriForce, row_mid + 1, column_mid + 1, row_end, column_end))
        {
            return 0;
        }

        topLeft = calculateTriForce(checkTriForce, currentSquareSize / 2, row_start, column_start, row_mid, column_mid);
        topRight = calculateTriForce(checkTriForce, currentSquareSize / 2, row_start, column_mid + 1, row_mid, column_end);
        bottomLeft = calculateTriForce(checkTriForce, currentSquareSize / 2, row_mid + 1, column_start, row_end, column_mid);

        if (hasZero(topLeft, topRight, bottomLeft))
        {
            return 0;
        }

        return mostFrequentValue(topLeft, topRight, bottomLeft);
    }
}

int main()
{
    std::vector<int> answer(3);

    std::vector<std::vector<int>> firstTriForce = triForceInput();
    answer[0] = calculateTriForce(firstTriForce, firstTriForce.size(), 0, 0, firstTriForce.size() - 1, firstTriForce.size() - 1);
    std::vector<std::vector<int>> secondTriForce = triForceInput();
    answer[1] = calculateTriForce(secondTriForce, secondTriForce.size(), 0, 0, secondTriForce.size() - 1, secondTriForce.size() - 1);
    std::vector<std::vector<int>> thirdTriForce = triForceInput();
    answer[2] = calculateTriForce(thirdTriForce, thirdTriForce.size(), 0, 0, thirdTriForce.size() - 1, thirdTriForce.size() - 1);

    for (int value : answer)
    {
        std::cout << value << "\n";
    }
}