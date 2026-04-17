#include <iostream>
#include <vector>

void DFS(std::vector<int> & workerEff, std::vector<bool> & usedWorkers, std::vector<std::vector<int>> & workerRelation, int & currentMaxEff, int currentEff, int tailSum, int currentWorker, int & workers)
{
    currentMaxEff = std::max(currentMaxEff, currentEff);

    if (currentWorker == workers)
    {
        return;
    }

    if (currentEff + tailSum <= currentMaxEff)
    {
        return;
    }

    bool suitable = true;

    for (int j = 0; j < currentWorker; ++j) 
    {
        if (usedWorkers[j] == true && workerRelation[currentWorker][j] == 0)
        {
            suitable = false;
            break;
        }
    }

    if (suitable)
    {
        usedWorkers[currentWorker] = true;
        DFS(workerEff, usedWorkers, workerRelation, currentMaxEff, currentEff + workerEff[currentWorker], tailSum - workerEff[currentWorker], currentWorker + 1, workers);
        usedWorkers[currentWorker] = false;
    }

    DFS(workerEff, usedWorkers, workerRelation, currentMaxEff, currentEff, tailSum - workerEff[currentWorker], currentWorker + 1, workers);
}

int main()
{
    int workers, effSum = 0, currentMaxEff = 0;
    std::cin >> workers;
    std::vector<int> workerEff(workers);
    std::vector<bool> usedWorkers(workers, false);
    std::vector<std::vector<int>> workerRelation(workers, std::vector<int>(workers));

    for (int i = 0; i < workers; ++i)
    {
        std::cin >> workerEff[i];
        effSum += workerEff[i];
    }

    for (int i = 0; i < workers; ++i)
    {
        for (int j = 0; j < workers; ++j)
        {
            std::cin >> workerRelation[i][j];
        }
    }

    DFS(workerEff, usedWorkers, workerRelation, currentMaxEff, 0, effSum, 0, workers);

    std::cout << currentMaxEff;
}