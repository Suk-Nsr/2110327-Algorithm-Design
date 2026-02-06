#include <iostream>
#include <vector>

std::vector<int> multiplyMatrix(std::vector<int> A, std::vector<int> B, int modulo)
{
    std::vector<int> answer(4);

    answer[0] = (((A[0] * B[0]) % modulo) + ((A[1] * B[2]) % modulo)) % modulo;
    answer[1] = (((A[0] * B[1]) % modulo) + ((A[1] * B[3]) % modulo)) % modulo;
    answer[2] = (((A[2] * B[0]) % modulo) + ((A[3] * B[2]) % modulo)) % modulo;
    answer[3] = (((A[2] * B[1]) % modulo) + ((A[3] * B[3]) % modulo)) % modulo;

    return answer;
}

int main()
{
    int power, modulo;
    std::cin >> power >> modulo;

    std::vector<int> result {1,0,0,1}; //identity matrix (act like 1)
    std::vector<int> base (4);

    for (int i = 0; i < 4; ++i)
    {
        int input;
        std::cin >> input;
        input = input % modulo;
        base[i] = input;
    }

    while (power > 0)
    {
        if (power & 1)
        {
            result = multiplyMatrix(result, base, modulo);
        }

        base = multiplyMatrix(base, base, modulo);
        power = power >> 1;
    }

    for (int value : result)
    {
        std::cout << value << " ";
    }
}