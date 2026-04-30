#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <cmath>
#include <algorithm>

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

// 1. ระบบเรดาร์ (BFS) สแกนระยะทางจริงที่หลบกำแพงแล้ว
vector<int> bfs_distances(int start_r, int start_c) {
    vector<int> dist(N * N, 1e9); 
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

// 2. A* Search สำหรับลากทีละแผ่น (เร็ว ทะลุทะลวง ไม่กิน RAM)
string solve_single_tile(int start_tr, int start_tc, int start_er, int start_ec, int goal_r, int goal_c) {
    priority_queue<Node, vector<Node>, greater<Node>> pq;
    visit_token++; 
    
    int h_start = 2 * (abs(start_tr - goal_r) + abs(start_tc - goal_c)) 
                  + (abs(start_tr - start_er) + abs(start_tc - start_ec));
                  
    pq.push({start_tr, start_tc, start_er, start_ec, 0, h_start, ""});

    // ทิศทางอิงตามการเลื่อนกระเบื้อง (ตรงกับ Visualizer)
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
                // ห้ามเดินทับกำแพง (แผ่นที่ล็อกแล้ว)
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

    // --- สร้างเป้าหมายแบบก้นหอยอัจฉริยะ (Offset-Corner Inside-Out Spiral) ---
    vector<pair<int, int>> target_order;
    vector<vector<pair<int, int>>> rings; 

    int top_edge = 1, bottom_edge = N - 2;
    int left_edge = 1, right_edge = N - 2;

    // 1. สร้างวงแหวนจากวงนอกสุด เข้าไปหาวงในสุด
    while (top_edge <= bottom_edge && left_edge <= right_edge) {
        vector<pair<int, int>> current_ring;
        
        if (top_edge == bottom_edge && left_edge == right_edge) {
            // กรณีตรงกลางสุดเป็นจุดเดียว (สำหรับกระดานคี่)
            current_ring.push_back({top_edge, left_edge});
        } else {
            // เริ่มที่ "ถัดจากมุมซ้ายบน 1 ช่อง" แล้ววนขวาตามเข็มนาฬิกา
            for (int c = left_edge + 1; c <= right_edge; ++c) current_ring.push_back({top_edge, c});
            for (int r = top_edge + 1; r <= bottom_edge; ++r) current_ring.push_back({r, right_edge});
            for (int c = right_edge - 1; c >= left_edge; --c) current_ring.push_back({bottom_edge, c});
            for (int r = bottom_edge - 1; r > top_edge; --r) current_ring.push_back({r, left_edge});
            
            // จบที่ "มุมซ้ายบนสุด" เป็นแผ่นสุดท้ายของวงแหวน! (ทฤษฎี 2 ประตู)
            current_ring.push_back({top_edge, left_edge});
        }
        rings.push_back(current_ring);
        
        // บีบขอบเขตเข้ามา 1 ชั้น
        top_edge++; bottom_edge--; left_edge++; right_edge--;
    }

    // 2. พลิกจากวงในสุด ออกมาวงนอกสุด (Inside-Out)
    reverse(rings.begin(), rings.end());
    for (const auto& ring : rings) {
        for (const auto& p : ring) {
            target_order.push_back(p);
        }
    }

    string final_answer = "";
    cerr << "[INFO] Starting Ultimate Offset-Corner Spiral Solver...\n";

    int count_done = 0;
    int total_targets = (N - 2) * (N - 2);

    for (auto goal_pos : target_order) {
        int i = goal_pos.first;
        int j = goal_pos.second;
        int goal_val = target_flat[(i - 1) * (N - 2) + (j - 1)];

        if (grid[get_idx(i, j)] == goal_val) {
            locked[get_idx(i, j)] = true;
            count_done++;
            continue;
        }

        vector<int> dist_from_goal = bfs_distances(i, j);
        vector<int> dist_from_empty = bfs_distances(er, ec);

        int best_tr = -1, best_tc = -1;
        int min_cost = 2e9;

        for (int r = 0; r < N; ++r) {
            for (int c = 0; c < N; ++c) {
                if (grid[get_idx(r, c)] == goal_val && !locked[get_idx(r, c)]) {
                    
                    if (dist_from_goal[get_idx(r, c)] == 1e9 || dist_from_empty[get_idx(r, c)] == 1e9) continue;

                    bool is_perfect_match = false;
                    if (r >= 1 && r <= N - 2 && c >= 1 && c <= N - 2) {
                        if (grid[get_idx(r, c)] == target_flat[(r - 1) * (N - 2) + (c - 1)]) {
                            is_perfect_match = true; 
                        }
                    }

                    int penalty = is_perfect_match ? 10000 : 0;
                    
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
            cerr << "[FATAL] Blocked! The radar couldn't find a path.\n";
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

    final_answer = optimize_path(final_answer);

    cerr << "\n\n[SUCCESS] Board solved completely!\n";
    cout << "\n-----------------------------------\n";
    cout << "Total Moves: " << final_answer.length() << " (excluding 'S')\n";
    cout << final_answer << "S\n";

    return 0;
}