#include <iostream>
#include <vector>
#include <queue>

using namespace std;

struct Node {
    int r;
    int c;
    int weight;

    bool operator < (const Node & other) const {
        return weight > other.weight;
    }
};

int runDijkstra(const vector<vector<int>>& adj, int startRow, int startColumn, int targetRow, int targetColumn, int row, int column)
{
    vector<vector<int>> distance(row, vector<int>(column, 1e9));
    priority_queue<Node> pq;

    int startCost = adj[startRow][startColumn];
    pq.push({startRow, startColumn, startCost});
    distance[startRow][startColumn] = startCost;

    int dr_odd_prob[6] = {0, 0, -1, -1, 1, 1};
    int dc_odd_prob[6] = {-1, 1, 0, 1, 0, 1};

    int dr_even_prob[6] = {0, 0, -1, -1, 1, 1};
    int dc_even_prob[6] = {-1, 1, -1, 0, -1, 0};

    while(!pq.empty()) {
        int r = pq.top().r;
        int c = pq.top().c;
        int weight = pq.top().weight;
        pq.pop();

        if (weight > distance[r][c]) {
            continue;
        }

        if (r == targetRow && c == targetColumn) {
            return weight;
        }

        const int* dr = (r % 2 == 0) ? dr_odd_prob : dr_even_prob;
        const int* dc = (r % 2 == 0) ? dc_odd_prob : dc_even_prob;

        for (int i = 0; i < 6; ++i) {
            int nr = r + dr[i];
            int nc = c + dc[i];

            if (nr >= 0 && nr < row && nc >= 0 && nc < column) {
                int nextCost = weight + adj[nr][nc];
                if (nextCost < distance[nr][nc]) {
                    distance[nr][nc] = nextCost;
                    pq.push({nr, nc, nextCost});
                }
            }
        }
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int row, column;
    if (!(cin >> row >> column)) return 0;
    
    int startRow, startColumn, targetRow, targetColumn;
    cin >> startRow >> startColumn >> targetRow >> targetColumn;

    startRow--; startColumn--;
    targetRow--; targetColumn--;

    vector<vector<int>> adj(row, vector<int>(column));

    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < column; ++j) {
            cin >> adj[i][j];
        }
    }

    cout << runDijkstra(adj, startRow, startColumn, targetRow, targetColumn, row, column);
}