#include <iostream>
#include <vector>

int answer = 0;

void findNode(std::vector<std::vector<int>> & adjacentNodes, std::vector<std::vector<int>> & nodeCountWithDistance, int currentNode, int distanceCheck)
{
    nodeCountWithDistance[currentNode][0] = 1;

    for (int node : adjacentNodes[currentNode])
    {
        findNode(adjacentNodes, nodeCountWithDistance, node, distanceCheck);

        for (int subDistance = 0; subDistance < distanceCheck; ++subDistance)
        {
            if (distanceCheck - 1 - subDistance >= 0)
            {
                answer += nodeCountWithDistance[currentNode][distanceCheck - 1 - subDistance] * nodeCountWithDistance[node][subDistance];
            }
        }

        for (int subDistance = 0; subDistance < distanceCheck; ++subDistance)
        {
            nodeCountWithDistance[currentNode][subDistance + 1] += nodeCountWithDistance[node][subDistance];
        }
    }
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    int nodeCount, distanceCheck;
    std::cin >> nodeCount >> distanceCheck;
    std::vector<std::vector<int>> adjacentNodes(nodeCount + 1);
    std::vector<std::vector<int>> nodeCountWithDistance(nodeCount + 1, std::vector<int>(distanceCheck + 1, 0));

    for (int i = 0; i < nodeCount - 1; ++i)
    {
        int parentNode, childNode;
        std::cin >> parentNode >> childNode;
        adjacentNodes[parentNode].push_back(childNode);
    }

    findNode(adjacentNodes, nodeCountWithDistance, 1, distanceCheck);

    std::cout << answer;
}