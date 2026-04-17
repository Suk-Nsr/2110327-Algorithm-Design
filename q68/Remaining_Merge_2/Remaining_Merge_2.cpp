#include <bits/stdc++.h>

unsigned long long total_count = 0;

int merge_sort(const std::string & bitString, int l, int r) {
    if (l >= r) {
        return (bitString[l] == '1') ? 1 : 0;
    }

    int m = (l + r) / 2;
    
    int leftCount = merge_sort(bitString, l, m);
    int rightCount = merge_sort(bitString, m + 1, r);

    if (leftCount == 0)
    {
        total_count += (r - m);
    }
    else if (rightCount == 0)
    {
        total_count += leftCount;
    }
    else
    {
        total_count += rightCount;
    }

    return leftCount + rightCount;
}

int main()
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0);

    int length;
    std::cin >> length;

    std::string bitString;
    std::cin >> bitString;

    merge_sort(bitString, 0, length - 1);

    std::cout << total_count;
}