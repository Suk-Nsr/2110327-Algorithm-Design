#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <cmath>

using namespace std;

int N;
vector<int> grid;
vector<int> target_flat;
vector<bool> locked;

vector<int> visited;
int visit_token = 0; 

inline int get_idx(int r, int c) { return r * N + c; }

struct Node {
    int tr, tc; 
    int er, ec; 
    int g, f;
    string path;

    bool operator>(const Node& other) const {
        if (f != other.f) return f > other.f;
        return g < other.g; 
    }
};

// --- ระบบ True Radar (BFS) ---
// หาระยะทางที่เดินได้จริง (หลบกำแพง) จากจุดเริ่มต้นไปยังทุกช่อง
vector<int> bfs_distances(int start_r, int start_c) {
    vector<int> dist(N * N, 1e9); // 1e9 คือเข้าไม่ถึง
    queue<pair<int, int>> q;
    
    q.push({start_r, start_c});
    dist[get_idx(start_r, start_c)] = 0;
    
    int dr[] = {1, -1, 0, 0};
    int dc[] = {0, 0, 1, -1};
    
    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();
        
        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i];
            int nc = c + dc[i];
            // เดินได้เฉพาะช่องที่ไม่หลุดกรอบ และ ไม่ถูกล็อก (ไม่ใช่กำแพง)
            if (nr >= 0 && nr < N && nc >= 0 && nc < N) {
                if (!locked[get_idx(nr, nc)]) {
                    if (dist[get_idx(nr, nc)] == 1e9) {
                        dist[get_idx(nr, nc)] = dist[get_idx(r, c)] + 1;
                        q.push({nr, nc});
                    }
                }
            }
        }
    }
    return dist;
}

// ฟังก์ชัน A* สำหรับลากกระเบื้อง 1 แผ่น
string solve_single_tile(int start_tr, int start_tc, int start_er, int start_ec, int goal_r, int goal_c) {
    priority_queue<Node, vector<Node>, greater<Node>> pq;
    visit_token++; 
    
    int h_start = 2 * (abs(start_tr - goal_r) + abs(start_tc - goal_c)) 
                  + (abs(start_tr - start_er) + abs(start_tc - start_ec));
                  
    pq.push({start_tr, start_tc, start_er, start_ec, 0, h_start, ""});

    // ทิศทางที่ถูกต้อง (อิงตามการเลื่อนกระเบื้อง) ตรงกับ Visualizer เป๊ะ!
    int dr[] = {1, -1, 0, 0};
    int dc[] = {0, 0, 1, -1};
    char move_char[] = {'U', 'D', 'L', 'R'};

    while (!pq.empty()) {
        Node curr = pq.top();
        pq.pop();

        if (curr.tr == goal_r && curr.tc == goal_c) {
            return curr.path;
        }

        int state_id = ((curr.tr * N + curr.tc) * N + curr.er) * N + curr.ec;
        if (visited[state_id] == visit_token) continue;
        visited[state_id] = visit_token;

        for (int i = 0; i < 4; ++i) {
            int ner = curr.er + dr[i];
            int nec = curr.ec + dc[i];

            if (ner >= 0 && ner < N && nec >= 0 && nec < N) {
                if (locked[get_idx(ner, nec)] && !(ner == curr.tr && nec == curr.tc)) continue;

                int ntr = curr.tr;
                int ntc = curr.tc;

                if (ner == curr.tr && nec == curr.tc) {
                    ntr = curr.er;
                    ntc = curr.ec;
                }

                int n_state_id = ((ntr * N + ntc) * N + ner) * N + nec;
                if (visited[n_state_id] != visit_token) {
                    int ng = curr.g + 1;
                    int nh = 2 * (abs(ntr - goal_r) + abs(ntc - goal_c)) + (abs(ntr - ner) + abs(ntc - nec));
                    pq.push({ntr, ntc, ner, nec, ng, ng + nh, curr.path + move_char[i]});
                }
            }
        }
    }
    return "FAILED";
}

void apply_path(const string& path, int& er, int& ec) {
    for (char m : path) {
        int ner = er, nec = ec;
        if (m == 'U') ner++;
        else if (m == 'D') ner--;
        else if (m == 'L') nec++;
        else if (m == 'R') nec--;
        
        swap(grid[get_idx(er, ec)], grid[get_idx(ner, nec)]);
        er = ner;
        ec = nec;
    }
}

string optimize_path(const string& path) {
    string res = "";
    for (char c : path) {
        if (!res.empty()) {
            char last = res.back();
            if ((last == 'U' && c == 'D') || (last == 'D' && c == 'U') ||
                (last == 'L' && c == 'R') || (last == 'R' && c == 'L')) {
                res.pop_back(); 
                continue;
            }
        }
        res.push_back(c);
    }
    return res;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    if (!(cin >> N)) return 0;

    grid.assign(N * N, 0);
    locked.assign(N * N, false);
    visited.assign(N * N * N * N, 0); 
    
    int er = -1, ec = -1; 

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cin >> grid[get_idx(i, j)];
            if (grid[get_idx(i, j)] == -1) {
                er = i;
                ec = j;
            }
        }
    }

    target_flat.assign((N - 2) * (N - 2), 0);
    for (int i = 0; i < N - 2; ++i) {
        for (int j = 0; j < N - 2; ++j) {
            cin >> target_flat[i * (N - 2) + j];
        }
    }

    string final_answer = "";
    cerr << "[INFO] Starting Super Fast Solver (with True Radar)...\n";

    int count_done = 0;
    int total_targets = (N - 2) * (N - 2);

    // ทำงานแบบ Snake Pattern ป้องกันการติดทางตัน
    for (int i = 1; i <= N - 2; ++i) {
        for (int step = 1; step <= N - 2; ++step) {
            int j = (i % 2 != 0) ? step : (N - 2 - step + 1); 
            int goal_val = target_flat[(i - 1) * (N - 2) + (j - 1)];

            // [อัปเกรด 1] เช็คก่อนเลย! ถ้ามันวางถูกสีอยู่แล้ว ล็อกมันทันที ไม่ต้องขยับให้เสีย Move
            if (grid[get_idx(i, j)] == goal_val) {
                locked[get_idx(i, j)] = true;
                count_done++;
                continue;
            }

            // [อัปเกรด 2] ใช้เรดาร์ BFS ยิงทะลุกระดานเพื่อหาระยะที่แท้จริง
            vector<int> dist_from_goal = bfs_distances(i, j);
            vector<int> dist_from_empty = bfs_distances(er, ec);

            int best_tr = -1, best_tc = -1;
            int min_cost = 2e9;

            for (int r = 0; r < N; ++r) {
                for (int c = 0; c < N; ++c) {
                    if (grid[get_idx(r, c)] == goal_val && !locked[get_idx(r, c)]) {
                        
                        // ถ้าระยะเป็น 1e9 แปลว่ามันถูกขังอยู่ในกำแพง ตัดทิ้งไปเลย!
                        if (dist_from_goal[get_idx(r, c)] == 1e9 || dist_from_empty[get_idx(r, c)] == 1e9) continue;

                        bool is_perfect_match = false;
                        if (r >= 1 && r <= N - 2 && c >= 1 && c <= N - 2) {
                            if (grid[get_idx(r, c)] == target_flat[(r - 1) * (N - 2) + (c - 1)]) {
                                is_perfect_match = true; // แผ่นนี้อยู่ในอนาคตที่ถูกต้องแล้ว
                            }
                        }

                        // Penalty ห้ามขโมยแผ่นที่วางถูกที่แล้ว
                        int penalty = is_perfect_match ? 10000 : 0;
                        
                        // คำนวณ Cost จากระยะทาง "ตามความเป็นจริง" (ฉลาดสุดๆ)
                        int cost = dist_from_empty[get_idx(r, c)] + (5 * dist_from_goal[get_idx(r, c)]) + penalty;
                        
                        if (cost < min_cost) {
                            min_cost = cost;
                            best_tr = r;
                            best_tc = c;
                        }
                    }
                }
            }

            string path = solve_single_tile(best_tr, best_tc, er, ec, i, j);
            
            if (path == "FAILED") {
                cerr << "[FATAL] A* couldn't route. This shouldn't happen mathematically!\n";
                return 0;
            }

            apply_path(path, er, ec);
            final_answer += path;
            locked[get_idx(i, j)] = true;

            count_done++;
            if (count_done % 10 == 0 || count_done == total_targets) {
                cerr << "[PROGRESS] Solved " << count_done << "/" << total_targets << " tiles...    \r";
            }
        }
    }

    final_answer = optimize_path(final_answer);

    cerr << "\n\n[SUCCESS] Board solved completely!\n";
    cout << "\n-----------------------------------\n";
    cout << "Total Moves: " << final_answer.length() << " (excluding 'S')\n";
    cout << final_answer << "S\n";

    return 0;
}