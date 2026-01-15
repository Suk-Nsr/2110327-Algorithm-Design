#include <iostream>
#include <vector>

int permutationSize, permissionCount;

std::vector<int> permission;
std::vector<int> currentPermutation;
std::vector<bool> used;

void recur(int depth)
{
    if (depth == permutationSize)
    {
        for (int value : currentPermutation)
        {
            std::cout << value << " ";
        }
        std::cout << "\n";
    }
    else
    {
        for (int i = 0; i < permutationSize; ++i)
        {
            if (!used[i])
            {
                if (permission[i] != -1 && !used[permission[i]])
                {
                    continue;
                }
            used[i] = true;
            currentPermutation[depth] = i;
            recur(depth + 1);
            used[i] = false;
            }
        }
    }
}

int main()
{
    std::cin >> permutationSize >> permissionCount;
    permission.assign(permutationSize, -1);
    currentPermutation.resize(permutationSize);
    used.assign(permutationSize, false);

    for (int i = 0; i < permissionCount; ++i)
    {
        int before, after;
        std::cin >> before >> after;
        permission[after] = before;
    }

    recur(0);
}