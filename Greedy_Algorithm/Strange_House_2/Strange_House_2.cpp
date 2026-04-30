#include <iostream>
#include <vector>

int main()
{
    int n;
    std::cin >> n;

    std::vector<int> a(2 * n);

    for (int i = 1; i <= 2 * n - 1; i++)
    {
        std::cin >> a[i];
    }

    std::vector<int> L_stack;

    for (int i = 1; i <= n - 1; i++)
    {
        L_stack.push_back(a[i]);
    }

    std::vector<int> R_stack;
    int misplaced = 0;

    for (int i = n + 1; i <= 2 * n - 1; i++)
    {
        R_stack.push_back(a[i]);

        if (a[i] == 1)
        {
            misplaced++;
        }
    }

    int current_n = a[n];
    std::vector<int> ops;

    while (misplaced > 0)
    {
        if (current_n == 1)
        {
            ops.push_back(1);

            if (!L_stack.empty())
            {
                current_n = L_stack.back();
                L_stack.pop_back();
            }
            else
            {
                current_n = 1;
            }
        }
        else
        {
            ops.push_back(2);

            if (!R_stack.empty())
            {
                current_n = R_stack.back();
                R_stack.pop_back();

                if (current_n == 1)
                {
                    misplaced--;
                }
            }
            else
            {
                current_n = 2;
            }
        }
    }

    std::cout << ops.size() << "\n";

    for (int i = 0; i < ops.size(); i++)
    {
        std::cout << ops[i] << (i == ops.size() - 1 ? "" : " ");
    }

    std::cout << "\n";

    return 0;
}