#include <iostream>
#include <vector>

//a_i = 1 + a_{i - a_{a_{i-1}}}

int main()
{
    std::vector<long long> values;
    std::vector<int> indexs;
    
    values.push_back(0);
    values.push_back(1);
    values.push_back(2);
    indexs.push_back(0);
    indexs.push_back(1);
    indexs.push_back(3);
    
    int i = 3;
    
    while (indexs.back() < 2000000000)
    {
        int value = 1 + values[i - values[values[i - 1]]];

        values.push_back(value);
        indexs.push_back(indexs.back() + value);
        
        ++i;
    }

    int N;
    std::cin >> N;

    for (int i = 0; i < N; ++i)
    {
        int searchedIndex;
        std::cin >> searchedIndex;
        std::vector<int>::iterator it = std::lower_bound(indexs.begin(), indexs.end(), searchedIndex);
        int answer = std::distance(indexs.begin(), it);
        std::cout << answer << "\n";
    }
}