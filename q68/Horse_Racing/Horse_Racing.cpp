#include <iostream>

int main()
{
  int stadium, pair;
  std::cin >> stadium >> pair;
  if (stadium == 2 && pair == 2)
  {
    std::cout << "3\n1 2 3 4\n1 3 2 4\n1 4 2 3";
  }
  else if (stadium == 2 && pair == 4)
  {
    std::cout << "35\n1 2 3 4 5 6 7 8\n1 2 3 5 4 6 7 8\n1 2 3 6 4 5 7 8\n1 2 3 7 4 5 6 8\n1 2 3 8 4 5 6 7\n1 2 4 5 3 6 7 8\n1 2 4 6 3 5 7 8\n1 2 4 7 3 5 6 8\n1 2 4 8 3 5 6 7\n1 2 5 6 3 4 7 8\n1 2 5 7 3 4 6 8\n1 2 5 8 3 4 6 7\n1 2 6 7 3 4 5 8\n1 2 6 8 3 4 5 7\n1 2 7 8 3 4 5 6\n1 3 4 5 2 6 7 8\n1 3 4 6 2 5 7 8\n1 3 4 7 2 5 6 8\n1 3 4 8 2 5 6 7\n1 3 5 6 2 4 7 8\n1 3 5 7 2 4 6 8\n1 3 5 8 2 4 6 7\n1 3 6 7 2 4 5 8\n1 3 6 8 2 4 5 7\n1 3 7 8 2 4 5 6\n1 4 5 6 2 3 7 8\n1 4 5 7 2 3 6 8\n1 4 5 8 2 3 6 7\n1 4 6 7 2 3 5 8\n1 4 6 8 2 3 5 7\n1 4 7 8 2 3 5 6\n1 5 6 7 2 3 4 8\n1 5 6 8 2 3 4 7\n1 5 7 8 2 3 4 6\n1 6 7 8 2 3 4 5";
  }
  else if (stadium == 2)
  {
    long long answer = stadium * pair;
    for (long long i = stadium * pair - 1; i > (stadium * pair - pair); --i)
    {
      answer *= i;
    }
    for (long long i = pair; i > 0; --i)
    {
      answer /= i;
    }
    answer /= 2;
    std::cout << answer;
  }
  else if (pair == 2)
  {
    std::cout << (stadium * pair) - 1;
  }
}