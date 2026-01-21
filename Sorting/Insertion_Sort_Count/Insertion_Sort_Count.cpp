#include <iostream>
#include <vector>

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);    

    int vectorSize, outputSize;
    std::cin >> vectorSize >> outputSize;
    std::vector<int> data(vectorSize);

    for (int i = 0; i < vectorSize; ++i)
    {
        std::cin >> data[i];
    }

    for (int i = 0; i < outputSize; ++i)
    {
        int checkedNumber, count = 0;
        std::cin >> checkedNumber;

        for (int k = 0; k < vectorSize; ++k)
        {
            if (data[k] == checkedNumber)
            {
                break;
            }
            if (data[k] > checkedNumber)
            {
                ++count;
            }
        }

        std::cout << count << "\n";
    }
}