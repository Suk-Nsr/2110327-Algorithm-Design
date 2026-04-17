#include <iostream>
#include <vector>

void posCheck(std::vector<int> & queenPos, std::vector<bool> & queenUsed, int & possiblePos, int currentPlaced, int N)
{
    for (int i = 1; i < currentPlaced; ++i)
    {
        if (queenPos[currentPlaced - 1] - i == queenPos[currentPlaced - 1 - i] || queenPos[currentPlaced - 1] + i == queenPos[currentPlaced - 1 - i])
        {
            queenPos[currentPlaced - 1] = -1;
            return;
        }
    }

    for (int i = 0; i < N; ++i)
    {
        if (queenUsed[i] == false)
        {
            queenPos[currentPlaced] = i + 1;
            queenUsed[i] = true;
            posCheck(queenPos, queenUsed, possiblePos, currentPlaced + 1, N);
            queenUsed[i] = false;
        }
    }

    if (currentPlaced == N)
    {
        ++possiblePos;
    }

    return;
}

int main()
{
    int N, possiblePos = 0;
    std::cin >> N;
    std::vector<int> queenPos(N, -1);
    std::vector<bool> queenUsed(N, false);

    posCheck(queenPos, queenUsed, possiblePos, 0, N);

    std::cout << possiblePos;
}