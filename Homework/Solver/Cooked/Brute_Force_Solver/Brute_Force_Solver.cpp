#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_set>
#include <cstdint>

using namespace std;

int N;
vector<int> target_flat;

// ฟังก์ชันหา Index ของ Array 1D
inline int get_idx(int r, int c) { return r * N + c; }

// ฟังก์ชันตรวจสอบชัยชนะ
bool isGoal(const vector<int8_t>& grid) {
    for (int i = 0; i < N - 2; ++i) {
        for (int j = 0; j < N - 2; ++j) {
            // เทียบค่ากระดานตรงกลาง กับเป้าหมาย
            if (grid[get_idx(i + 1, j + 1)] != target_flat[i * (N - 2) + j]) {
                return false;
            }
        }
    }
    return true;
}

// ฟังก์ชัน Hash สำหรับ unordered_set เพื่อให้ทำงานได้เร็ว
struct VectorHash {
    size_t operator()(const vector<int8_t>& v) const {
        size_t hash = 0;
        for (int8_t x : v) {
            hash ^= x + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

struct State {
    vector<int8_t> grid;
    int gap_r, gap_c;
    string path;
};

// อัลกอริทึม BFS
string solveBruteForceBFS(State start_state) {
    if (isGoal(start_state.grid)) return "S";

    queue<State> q;
    // ใช้ unordered_set เก็บกระดานที่เคยเจอแล้ว (เพื่อไม่ให้เดินวน)
    unordered_set<vector<int8_t>, VectorHash> visited;

    q.push(start_state);
    visited.insert(start_state.grid);

    int dr[] = {1, -1, 0, 0};
    int dc[] = {0, 0, 1, -1};
    char move_char[] = {'U', 'D', 'L', 'R'};

    int current_depth = 0;

    while (!q.empty()) {
        State curr = q.front();
        q.pop();

        // อัปเดต Progress ให้รู้ว่าคอมพิวเตอร์กำลังรันอยู่ลึกแค่ไหนแล้ว
        if (curr.path.length() > current_depth) {
            current_depth = curr.path.length();
            cerr << "[BFS] Searching Depth: " << current_depth 
                 << " | Queue Size: " << q.size() 
                 << " | Visited: " << visited.size() << " states   \r";
        }

        char last_move = curr.path.empty() ? ' ' : curr.path.back();

        for (int i = 0; i < 4; ++i) {
            // ตัดการเดินย้อนกลับ (Anti-reverse move)
            if ((last_move == 'U' && move_char[i] == 'D') ||
                (last_move == 'D' && move_char[i] == 'U') ||
                (last_move == 'L' && move_char[i] == 'R') ||
                (last_move == 'R' && move_char[i] == 'L')) continue;

            int nr = curr.gap_r + dr[i];
            int nc = curr.gap_c + dc[i];

            if (nr >= 0 && nr < N && nc >= 0 && nc < N) {
                State next_state = curr;
                
                swap(next_state.grid[get_idx(curr.gap_r, curr.gap_c)], next_state.grid[get_idx(nr, nc)]);
                
                // ถ้าเป็นกระดานรูปแบบใหม่ที่ยังไม่เคยเจอ
                if (visited.find(next_state.grid) == visited.end()) {
                    next_state.gap_r = nr;
                    next_state.gap_c = nc;
                    next_state.path += move_char[i];
                    
                    // เช็คว่าชนะไหม ถ้าชนะให้หยุดทันที!
                    if (isGoal(next_state.grid)) {
                        cerr << "\n\n[SUCCESS] Found OPTIMAL solution at depth " << next_state.path.length() << "!\n";
                        cerr << "[INFO] Total states explored: " << visited.size() << "\n";
                        return next_state.path + "S";
                    }

                    // จดจำแล้วเอาเข้าคิวไปค้นหาต่อ
                    visited.insert(next_state.grid);
                    q.push(next_state);
                }
            }
        }
    }

    return "No solution found (or Queue is empty)";
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    if (!(cin >> N)) return 0;

    State start_state;
    start_state.grid.assign(N * N, 0);
    
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int val;
            cin >> val;
            start_state.grid[get_idx(i, j)] = (int8_t)val;
            if (val == -1) {
                start_state.gap_r = i;
                start_state.gap_c = j;
            }
        }
    }

    target_flat.assign((N - 2) * (N - 2), 0);
    for (int i = 0; i < N - 2; ++i) {
        for (int j = 0; j < N - 2; ++j) {
            cin >> target_flat[i * (N - 2) + j];
        }
    }

    cerr << "[INFO] Starting PURE BRUTE FORCE (BFS) for N=" << N << "...\n";
    string answer = solveBruteForceBFS(start_state);

    cout << "\n-----------------------------------\n";
    if (answer.back() == 'S') {
        cout << "Moves Count: " << answer.length() - 1 << " (excluding 'S')\n";
    }
    cout << answer << "\n";

    return 0;
}