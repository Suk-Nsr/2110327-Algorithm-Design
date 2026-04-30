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

string solve_single_tile(int start_tr, int start_tc, int start_er, int start_ec, int goal_r, int goal_c) {
    priority_queue<Node, vector<Node>, greater<Node>> pq;
    visit_token++; 
    
    int h_start = 2 * (abs(start_tr - goal_r) + abs(start_tc - goal_c)) 
                  + (abs(start_tr - start_er) + abs(start_tc - start_ec));
                  
    pq.push({start_tr, start_tc, start_er, start_ec, 0, h_start, ""});

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

// 3. ฟังก์ชันบีบอัดเส้นทาง ตัดการขยับย้อนกลับที่ไร้ประโยชน์ทิ้ง
string optimize_path(const string& path) {
    string res = "";
    for (char c : path) {
        if (!res.empty()) {
            char last = res.back();
            if ((last == 'U' && c == 'D') || (last == 'D' && c == 'U') ||
                (last == 'L' && c == 'R') || (last == 'R' && c == 'L')) {
                res.pop_back(); // ยกเลิกกันเอง เอาออกคู่เลย
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
    cerr << "[INFO] Starting Optimized Macro-move Solver...\n";

    int count_done = 0;
    int total_targets = (N - 2) * (N - 2);

    for (int i = 1; i <= N - 2; ++i) {
        // 2. Snake Pattern: สลับการทำซ้ายไปขวา และ ขวามาซ้าย ตามแถวคู่/คี่
        for (int step = 1; step <= N - 2; ++step) {
            int j = (i % 2 != 0) ? step : (N - 2 - step + 1); // แถวคี่ไปขวา แถวคู่ไปซ้าย
            
            int goal_val = target_flat[(i - 1) * (N - 2) + (j - 1)];

            // 1. Smart Selection: นำระยะของช่องว่างมาคิดคำนวณด้วย
            int best_tr = -1, best_tc = -1;
            int min_cost = 1e9;
            for (int r = 0; r < N; ++r) {
                for (int c = 0; c < N; ++c) {
                    if (grid[get_idx(r, c)] == goal_val && !locked[get_idx(r, c)]) {
                        int dist_to_target = abs(r - i) + abs(c - j);
                        int dist_to_empty = abs(r - er) + abs(c - ec);
                        
                        // ถ่วงน้ำหนักการลาก 1 ช่องเท่ากับ 5 moves
                        int cost = dist_to_empty + (5 * dist_to_target);
                        
                        if (cost < min_cost) {
                            min_cost = cost;
                            best_tr = r;
                            best_tc = c;
                        }
                    }
                }
            }

            if (best_tr == i && best_tc == j) {
                locked[get_idx(i, j)] = true;
                count_done++;
                continue;
            }

            string path = solve_single_tile(best_tr, best_tc, er, ec, i, j);
            
            if (path == "FAILED") {
                cerr << "[ERROR] Cannot find path to tile!\n";
                return 0;
            }

            apply_path(path, er, ec);
            final_answer += path;
            locked[get_idx(i, j)] = true;

            count_done++;
            if (count_done % 10 == 0 || count_done == total_targets) {
                cerr << "[PROGRESS] Solved " << count_done << "/" << total_targets << " tiles...\r";
            }
        }
    }

    // เรียกใช้ฟังก์ชันบีบอัดเส้นทางก่อนส่งคำตอบ
    final_answer = optimize_path(final_answer);

    cerr << "\n[SUCCESS] Board solved successfully!\n";
    cout << "\n-----------------------------------\n";
    cout << "Optimized Moves Count: " << final_answer.length() << " (excluding 'S')\n";
    cout << final_answer << "S\n";

    return 0;
}