#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <cmath>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

using namespace std;

int N;
vector<int> original_grid;
vector<int> target_flat;

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

// ฟังก์ชันเรดาร์สแกนระยะทางที่หลบกำแพงแล้ว
vector<int> bfs_distances(int start_r, int start_c, const vector<bool>& locked) {
    vector<int> dist(N * N, 1e9); 
    queue<pair<int, int>> q;
    q.push({start_r, start_c});
    dist[get_idx(start_r, start_c)] = 0;
    
    int dr[] = {1, -1, 0, 0};
    int dc[] = {0, 0, 1, -1};
    
    while (!q.empty()) {
        auto [r, c] = q.front(); q.pop();
        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i], nc = c + dc[i];
            if (nr >= 0 && nr < N && nc >= 0 && nc < N && !locked[get_idx(nr, nc)]) {
                if (dist[get_idx(nr, nc)] == 1e9) {
                    dist[get_idx(nr, nc)] = dist[get_idx(r, c)] + 1;
                    q.push({nr, nc});
                }
            }
        }
    }
    return dist;
}

// A* Search แบบอัปเกรด (มี Linear Conflict Penalty ช่วยให้เดินไม่ชนกันเอง)
string solve_single_tile(int start_tr, int start_tc, int start_er, int start_ec, int goal_r, int goal_c, const vector<bool>& locked) {
    priority_queue<Node, vector<Node>, greater<Node>> pq;
    vector<int> visited(N * N * N * N, 0);
    int visit_token = 1;
    
    int h_start = 2 * (abs(start_tr - goal_r) + abs(start_tc - goal_c)) + (abs(start_tr - start_er) + abs(start_tc - start_ec));
    pq.push({start_tr, start_tc, start_er, start_ec, 0, h_start, ""});

    int dr[] = {1, -1, 0, 0};
    int dc[] = {0, 0, 1, -1};
    char move_char[] = {'U', 'D', 'L', 'R'};

    while (!pq.empty()) {
        Node curr = pq.top(); pq.pop();
        if (curr.tr == goal_r && curr.tc == goal_c) return curr.path;

        int state_id = ((curr.tr * N + curr.tc) * N + curr.er) * N + curr.ec;
        if (visited[state_id] == visit_token) continue;
        visited[state_id] = visit_token;

        for (int i = 0; i < 4; ++i) {
            int ner = curr.er + dr[i], nec = curr.ec + dc[i];
            if (ner >= 0 && ner < N && nec >= 0 && nec < N) {
                if (locked[get_idx(ner, nec)] && !(ner == curr.tr && nec == curr.tc)) continue;

                int ntr = curr.tr, ntc = curr.tc;
                if (ner == curr.tr && nec == curr.tc) { ntr = curr.er; ntc = curr.ec; }

                int n_state_id = ((ntr * N + ntc) * N + ner) * N + nec;
                if (visited[n_state_id] != visit_token) {
                    int ng = curr.g + 1;
                    
                    // Linear Conflict
                    int penalty = 0;
                    if (ntr == goal_r && ner == goal_r) {
                        if (goal_c > ntc && nec < ntc) penalty += 2;
                        if (goal_c < ntc && nec > ntc) penalty += 2;
                    }
                    if (ntc == goal_c && nec == goal_c) {
                        if (goal_r > ntr && ner < ntr) penalty += 2;
                        if (goal_r < ntr && ner > ntr) penalty += 2;
                    }

                    int nh = 2 * (abs(ntr - goal_r) + abs(ntc - goal_c)) + (abs(ntr - ner) + abs(ntc - nec)) + penalty;
                    pq.push({ntr, ntc, ner, nec, ng, ng + nh, curr.path + move_char[i]});
                }
            }
        }
    }
    return "FAILED";
}

string optimize_path(const string& path) {
    string res = "";
    for (char c : path) {
        if (!res.empty()) {
            char last = res.back();
            if ((last == 'U' && c == 'D') || (last == 'D' && c == 'U') ||
                (last == 'L' && c == 'R') || (last == 'R' && c == 'L')) {
                res.pop_back(); continue;
            }
        }
        res.push_back(c);
    }
    return res;
}

// ฟังก์ชันจำลองการแก้ปริศนา
string run_simulation(const vector<pair<int, int>>& target_order) {
    vector<int> grid = original_grid;
    vector<bool> locked(N * N, false);
    int er = -1, ec = -1;
    
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) { if (grid[get_idx(i, j)] == -1) { er = i; ec = j; } }
    }

    string final_answer = "";

    for (auto goal_pos : target_order) {
        int i = goal_pos.first;
        int j = goal_pos.second;
        int goal_val = target_flat[(i - 1) * (N - 2) + (j - 1)];

        if (grid[get_idx(i, j)] == goal_val) {
            locked[get_idx(i, j)] = true;
            continue;
        }

        vector<int> dist_from_goal = bfs_distances(i, j, locked);
        vector<int> dist_from_empty = bfs_distances(er, ec, locked);

        int best_tr = -1, best_tc = -1;
        int min_cost = 2e9;

        for (int r = 0; r < N; ++r) {
            for (int c = 0; c < N; ++c) {
                if (grid[get_idx(r, c)] == goal_val && !locked[get_idx(r, c)]) {
                    if (dist_from_goal[get_idx(r, c)] == 1e9 || dist_from_empty[get_idx(r, c)] == 1e9) continue;

                    bool is_perfect_match = (r >= 1 && r <= N - 2 && c >= 1 && c <= N - 2 && 
                                             grid[get_idx(r, c)] == target_flat[(r - 1) * (N - 2) + (c - 1)]);
                    
                    int cost = dist_from_empty[get_idx(r, c)] + (5 * dist_from_goal[get_idx(r, c)]) + (is_perfect_match ? 10000 : 0);
                    
                    if (cost < min_cost) {
                        min_cost = cost; best_tr = r; best_tc = c;
                    }
                }
            }
        }

        string path = solve_single_tile(best_tr, best_tc, er, ec, i, j, locked);
        if (path == "FAILED") return "FAILED";

        for (char m : path) {
            int ner = er, nec = ec;
            if (m == 'U') ner++; else if (m == 'D') ner--; else if (m == 'L') nec++; else if (m == 'R') nec--;
            swap(grid[get_idx(er, ec)], grid[get_idx(ner, nec)]);
            er = ner; ec = nec;
        }
        final_answer += path;
        locked[get_idx(i, j)] = true;
    }
    return optimize_path(final_answer);
}

int main() {
    ios_base::sync_with_stdio(false); cin.tie(NULL);
    if (!(cin >> N)) return 0;
    original_grid.assign(N * N, 0); target_flat.assign((N - 2) * (N - 2), 0);
    for (int i = 0; i < N; ++i) for (int j = 0; j < N; ++j) cin >> original_grid[get_idx(i, j)];
    for (int i = 0; i < N - 2; ++i) for (int j = 0; j < N - 2; ++j) cin >> target_flat[i * (N - 2) + j];

    int M = N - 2;
    vector<pair<string, vector<pair<int, int>>>> strategies;

    // ========================================================
    // 1. Horizontal Snake (4 ทิศทาง)
    // ========================================================
    vector<pair<int, int>> h_tl, h_tr, h_bl, h_br;
    for (int i = 1; i <= M; ++i) {
        for (int step = 1; step <= M; ++step) {
            h_tl.push_back({i, (i % 2 != 0) ? step : (M - step + 1)});
            h_tr.push_back({i, (i % 2 != 0) ? (M - step + 1) : step});
        }
    }
    for (int i = M, row = 1; i >= 1; --i, ++row) {
        for (int step = 1; step <= M; ++step) {
            h_bl.push_back({i, (row % 2 != 0) ? step : (M - step + 1)});
            h_br.push_back({i, (row % 2 != 0) ? (M - step + 1) : step});
        }
    }
    strategies.push_back({"Horizontal Snake (Top-Left)", h_tl});
    strategies.push_back({"Horizontal Snake (Top-Right)", h_tr});
    strategies.push_back({"Horizontal Snake (Bottom-Left)", h_bl});
    strategies.push_back({"Horizontal Snake (Bottom-Right)", h_br});

    // ========================================================
    // 2. Vertical Snake (4 ทิศทาง)
    // ========================================================
    vector<pair<int, int>> v_tl, v_tr, v_bl, v_br;
    for (int j = 1; j <= M; ++j) {
        for (int step = 1; step <= M; ++step) {
            v_tl.push_back({(j % 2 != 0) ? step : (M - step + 1), j});
            v_bl.push_back({(j % 2 != 0) ? (M - step + 1) : step, j});
        }
    }
    for (int j = M, col = 1; j >= 1; --j, ++col) {
        for (int step = 1; step <= M; ++step) {
            v_tr.push_back({(col % 2 != 0) ? step : (M - step + 1), j});
            v_br.push_back({(col % 2 != 0) ? (M - step + 1) : step, j});
        }
    }
    strategies.push_back({"Vertical Snake (Top-Left)", v_tl});
    strategies.push_back({"Vertical Snake (Bottom-Left)", v_bl});
    strategies.push_back({"Vertical Snake (Top-Right)", v_tr});
    strategies.push_back({"Vertical Snake (Bottom-Right)", v_br});

    // ========================================================
    // 3. Fringe / L-Shape (8 ทิศทาง - Row First & Col First)
    // ========================================================
    vector<pair<int, int>> f_tl_h, f_tl_v, f_tr_h, f_tr_v, f_bl_h, f_bl_v, f_br_h, f_br_v;
    for (int k = 1; k <= M; ++k) {
        int opp = M - k + 1; 

        // Top-Left (Row-First)
        for (int c = k; c <= M; ++c) f_tl_h.push_back({k, c});
        for (int r = k + 1; r <= M; ++r) f_tl_h.push_back({r, k});

        // Top-Left (Col-First)
        for (int r = k; r <= M; ++r) f_tl_v.push_back({r, k});
        for (int c = k + 1; c <= M; ++c) f_tl_v.push_back({k, c});

        // Top-Right (Row-First)
        for (int c = opp; c >= 1; --c) f_tr_h.push_back({k, c});
        for (int r = k + 1; r <= M; ++r) f_tr_h.push_back({r, opp});

        // Top-Right (Col-First)
        for (int r = k; r <= M; ++r) f_tr_v.push_back({r, opp});
        for (int c = opp - 1; c >= 1; --c) f_tr_v.push_back({k, c});

        // Bottom-Left (Row-First)
        for (int c = k; c <= M; ++c) f_bl_h.push_back({opp, c});
        for (int r = opp - 1; r >= 1; --r) f_bl_h.push_back({r, k});

        // Bottom-Left (Col-First)
        for (int r = opp; r >= 1; --r) f_bl_v.push_back({r, k});
        for (int c = k + 1; c <= M; ++c) f_bl_v.push_back({opp, c});

        // Bottom-Right (Row-First)
        for (int c = opp; c >= 1; --c) f_br_h.push_back({opp, c});
        for (int r = opp - 1; r >= 1; --r) f_br_h.push_back({r, opp});

        // Bottom-Right (Col-First)
        for (int r = opp; r >= 1; --r) f_br_v.push_back({r, opp});
        for (int c = opp - 1; c >= 1; --c) f_br_v.push_back({opp, c});
    }
    strategies.push_back({"Fringe (Top-Left, Row-First)", f_tl_h});
    strategies.push_back({"Fringe (Top-Left, Col-First)", f_tl_v});
    strategies.push_back({"Fringe (Top-Right, Row-First)", f_tr_h});
    strategies.push_back({"Fringe (Top-Right, Col-First)", f_tr_v});
    strategies.push_back({"Fringe (Bottom-Left, Row-First)", f_bl_h});
    strategies.push_back({"Fringe (Bottom-Left, Col-First)", f_bl_v});
    strategies.push_back({"Fringe (Bottom-Right, Row-First)", f_br_h});
    strategies.push_back({"Fringe (Bottom-Right, Col-First)", f_br_v});

    // ========================================================
    // 4. Diagonal Sweep / Staircase (8 ทิศทาง - 4 มุม x 2 แนวสแกน)
    // ========================================================
    vector<pair<int, int>> d_tl_1, d_tl_2, d_tr_1, d_tr_2;
    vector<pair<int, int>> d_bl_1, d_bl_2, d_br_1, d_br_2;

    for (int s = 2; s <= 2 * M; ++s) {
        vector<pair<int, int>> curr_tl, curr_tr, curr_bl, curr_br;
        
        // คำนวณขอบเขตของ Row ในเส้นทแยงมุมนี้
        int min_r = max(1, s - M);
        int max_r = min(M, s - 1);
        
        for (int r = min_r; r <= max_r; ++r) {
            int c = s - r;
            
            // มุมซ้ายบน (Top-Left)
            curr_tl.push_back({r, c});
            // มุมขวาบน (Top-Right) -> กลับด้านคอลัมน์
            curr_tr.push_back({r, M - c + 1});
            // มุมซ้ายล่าง (Bottom-Left) -> กลับด้านแถว
            curr_bl.push_back({M - r + 1, c});
            // มุมขวาล่าง (Bottom-Right) -> กลับด้านทั้งแถวและคอลัมน์
            curr_br.push_back({M - r + 1, M - c + 1});
        }

        // แบบที่ 1: สแกนตามลำดับปกติ
        for (auto p : curr_tl) d_tl_1.push_back(p);
        for (auto p : curr_tr) d_tr_1.push_back(p);
        for (auto p : curr_bl) d_bl_1.push_back(p);
        for (auto p : curr_br) d_br_1.push_back(p);

        // แบบที่ 2: สแกนย้อนกลับ (Reverse Sweep)
        reverse(curr_tl.begin(), curr_tl.end());
        for (auto p : curr_tl) d_tl_2.push_back(p);
        
        reverse(curr_tr.begin(), curr_tr.end());
        for (auto p : curr_tr) d_tr_2.push_back(p);
        
        reverse(curr_bl.begin(), curr_bl.end());
        for (auto p : curr_bl) d_bl_2.push_back(p);
        
        reverse(curr_br.begin(), curr_br.end());
        for (auto p : curr_br) d_br_2.push_back(p);
    }

    strategies.push_back({"Diagonal (Top-Left, Down-Left Sweep)", d_tl_1});
    strategies.push_back({"Diagonal (Top-Left, Up-Right Sweep)", d_tl_2});
    strategies.push_back({"Diagonal (Top-Right, Down-Right Sweep)", d_tr_1});
    strategies.push_back({"Diagonal (Top-Right, Up-Left Sweep)", d_tr_2});
    strategies.push_back({"Diagonal (Bottom-Left, Up-Left Sweep)", d_bl_1});
    strategies.push_back({"Diagonal (Bottom-Left, Down-Right Sweep)", d_bl_2});
    strategies.push_back({"Diagonal (Bottom-Right, Up-Right Sweep)", d_br_1});
    strategies.push_back({"Diagonal (Bottom-Right, Down-Left Sweep)", d_br_2});

    // ========================================================
    // 🔥 THE BATTLE ROYALE (16 Universes) 🔥
    // ========================================================
    cerr << "[INFO] Running Clean 24-Universe Ensemble (No F2L)...\n\n";

    string best_path = "";
    int min_len = 2e9;
    string best_name = "";

    for (const auto& strategy : strategies) {
        cerr << "Running " << strategy.first << "... ";
        string path = run_simulation(strategy.second);
        
        if (path != "FAILED") {
            cerr << path.length() << " moves\n";
            if (path.length() < min_len) {
                best_path = path;
                min_len = path.length();
                best_name = strategy.first;
            }
        } else {
            cerr << "FAILED\n";
        }
    }

    // (รัน 16 หรือ 24 Universes เสร็จเรียบร้อยแล้ว...)
    cerr << "\n[SUCCESS] The Winner is: " << best_name << " with " << best_path.length() << " moves!\n";    
    cout << "\n-----------------------------------\n";
    cout << "Best Moves: " << best_path.length() << " (excluding 'S')\n";
    cout << best_path << "S\n";

    return 0;
}