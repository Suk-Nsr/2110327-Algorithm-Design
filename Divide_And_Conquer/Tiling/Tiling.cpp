#include <iostream>
#include <vector>

void placementCalculation(std::vector<std::vector<int>> & answer, int rowStart, int rowEnd, int columnStart, int columnEnd, int missingX, int missingY)
{
    //Base case
    if (rowEnd - rowStart == 1)
    {
        if (missingX > columnStart)
        {
            if (missingY > rowStart)
            {
                std::vector<int> block {3, columnStart, rowStart};
                answer.push_back(block);
            }
            else
            {
                std::vector<int> block {1, columnStart, rowStart};
                answer.push_back(block);
            }
        }
        else
        {
            if (missingY > rowStart)
            {
                std::vector<int> block {2, columnStart, rowStart};
                answer.push_back(block);
            }
            else
            {
                std::vector<int> block {0, columnStart, rowStart};
                answer.push_back(block);
            }
        }
        return;
    }

    int rowMid = (rowStart + rowEnd) >> 1;
    int columnMid = (columnStart + columnEnd) >> 1;

    if (missingX > columnMid)
    {
        if (missingY > rowMid)
        {
            //Actual hole is in Quadrant 4
            //Place a d-type block in the middle
            std::vector<int> block {3, columnMid, rowMid};
            answer.push_back(block);
            //Recursive into a smaller section (Quadrant 4)
            //Call the same function again with the "Actual hole"
            placementCalculation(answer, rowMid + 1, rowEnd, columnMid + 1, columnEnd, missingX, missingY);
            //Recursive into a smaller section (Quadrant 1, 2, 3)
            //Call other parts with the "Virtual hole"
            placementCalculation(answer, rowStart, rowMid, columnStart, columnMid, columnMid, rowMid);
            placementCalculation(answer, rowStart, rowMid, columnMid + 1, columnEnd, columnMid + 1, rowMid);
            placementCalculation(answer, rowMid + 1, rowEnd, columnStart, columnMid, columnMid, rowMid + 1);
        }
        else
        {
            //Actual hole is in Quadrant 2
            //Place a b-type block in the middle
            std::vector<int> block {1, columnMid, rowMid};
            answer.push_back(block);
            //Recursive into a smaller section (Quadrant 2)
            //Call the same function again with the "Actual hole"
            placementCalculation(answer, rowStart, rowMid, columnMid + 1, columnEnd, missingX, missingY);
            //Recursive into a smaller section (Quadrant 1, 3, 4)
            //Call other parts with the "Virtual hole"
            placementCalculation(answer, rowStart, rowMid, columnStart, columnMid, columnMid, rowMid);
            placementCalculation(answer, rowMid + 1, rowEnd, columnStart, columnMid, columnMid, rowMid + 1);
            placementCalculation(answer, rowMid + 1, rowEnd, columnMid + 1, columnEnd, columnMid + 1, rowMid + 1);
        }
    }
    else
    {
        if (missingY > rowMid)
        {
            //Actual hole is in Quadrant 3
            //Place a c-type block in the middle
            std::vector<int> block {2, columnMid, rowMid};
            answer.push_back(block);
            //Recursive into a smaller section (Quadrant 3)
            //Call the same function again with the "Actual hole"
            placementCalculation(answer, rowMid + 1, rowEnd, columnStart, columnMid, missingX, missingY);
            //Recursive into a smaller section (Quadrant 1, 2, 4)
            //Call other parts with the "Virtual hole"
            placementCalculation(answer, rowStart, rowMid, columnStart, columnMid, columnMid, rowMid);
            placementCalculation(answer, rowStart, rowMid, columnMid + 1, columnEnd, columnMid + 1, rowMid);
            placementCalculation(answer, rowMid + 1, rowEnd, columnMid + 1, columnEnd, columnMid + 1, rowMid + 1);
        }
        else
        {
            //Actual hole is in Quadrant 1
            //Place a a-type block in the middle
            std::vector<int> block {0, columnMid, rowMid};
            answer.push_back(block);
            //Recursive into a smaller section (Quadrant 1)
            //Call the same function again with the "Actual hole"
            placementCalculation(answer, rowStart, rowMid, columnStart, columnMid, missingX, missingY);
            //Recursive into a smaller section (Quadrant 2, 3, 4)
            //Call other parts with the "Virtual hole"
            placementCalculation(answer, rowStart, rowMid, columnMid + 1, columnEnd, columnMid + 1, rowMid);
            placementCalculation(answer, rowMid + 1, rowEnd, columnStart, columnMid, columnMid, rowMid + 1);
            placementCalculation(answer, rowMid + 1, rowEnd, columnMid + 1, columnEnd, columnMid + 1, rowMid + 1);
        }
    }
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int roomSize, cordX, cordY;
    std::vector<std::vector<int>> answer;
    std::cin >> roomSize >> cordX >> cordY;

    placementCalculation(answer, 0, roomSize - 1, 0, roomSize - 1, cordX, cordY);

    std::cout << answer.size() << "\n";

    for (std::vector<int> block : answer)
    {
        for (int value : block)
        {
            std::cout << value << " ";
        }

        std::cout << "\n";
    }
}