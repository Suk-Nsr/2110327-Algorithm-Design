#include <iostream>
#include <vector>
#include <map>
#include <set>

std::map<char, std::set<char>> constraints;
std::vector<std::vector<char>> answers;
std::vector<char> answer;
int answerAmount = 0;

void permutation(int A, int B, int C, char prevChar, int index, int size)
{
    if (index == size)
    {
        ++answerAmount;
        answers.push_back(answer);
        return;
    }
    if (A && (constraints[prevChar].find('A') == constraints[prevChar].end()))
    {
        answer[index] = 'A';
        permutation(A - 1, B, C, 'A', index + 1, size);
    }
    if (B && (constraints[prevChar].find('B') == constraints[prevChar].end()))
    {
        answer[index] = 'B';
        permutation(A, B - 1, C, 'B', index + 1, size);
    }
    if (C && (constraints[prevChar].find('C') == constraints[prevChar].end()))
    {
        answer[index] = 'C';
        permutation(A, B, C - 1, 'C', index + 1, size);
    }
}

int main() 
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int size, A, B, C, constraintAmount;
    std::cin >> size >> A >> B >> C >> constraintAmount;
    answer.resize(size);

    for (int i = 0; i < constraintAmount; ++i)
    {
        char first, second;
        std::cin >> first >> second;
        constraints[first].insert(second);
    }

    if (A)
    {
        answer[0] = 'A';
        permutation(A - 1, B, C, 'A', 1, size);
    }

    if (B)
    {
        answer[0] = 'B';
        permutation(A, B - 1, C, 'B', 1, size);
    }

    if (C)
    {
        answer[0] = 'C';
        permutation(A, B, C - 1, 'C', 1, size);
    }

    std::cout << answerAmount << "\n";

    for (std::vector<char> subAnswer : answers)
    {
        for (char character : subAnswer)
        {
            std::cout << character;
        }

        std::cout << "\n";
    }
}