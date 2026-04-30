#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <cmath>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <iomanip>  // สำหรับจัดรูปแบบ Progress Bar
#include <fstream>  // สำหรับเขียนไฟล์ Backup

using namespace std;

// --- ตัวแปร Global ---
int N;
vector<int> original_grid;
vector<int> target_flat;
vector<unsigned long long> global_visited;
unsigned long long visit_token = 0; // <--- แก้ตรงนี้! ป้องกันระเบิดเวลา Overflow!

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

string solve_single_tile(int start_tr, int start_tc, int start_er, int start_ec, int goal_r, int goal_c, const vector<bool>& locked) {
    priority_queue<Node, vector<Node>, greater<Node>> pq;
    visit_token++; 
    
    int h_start = 5 * (abs(start_tr - goal_r) + abs(start_tc - goal_c)) + (abs(start_tr - start_er) + abs(start_tc - start_ec));
    pq.push({start_tr, start_tc, start_er, start_ec, 0, h_start, ""});

    int dr[] = {1, -1, 0, 0};
    int dc[] = {0, 0, 1, -1};
    char move_char[] = {'U', 'D', 'L', 'R'};

    while (!pq.empty()) {
        Node curr = pq.top(); pq.pop();
        if (curr.tr == goal_r && curr.tc == goal_c) return curr.path;

        int state_id = ((curr.tr * N + curr.tc) * N + curr.er) * N + curr.ec;
        if (global_visited[state_id] == visit_token) continue;
        global_visited[state_id] = visit_token;

        for (int i = 0; i < 4; ++i) {
            int ner = curr.er + dr[i], nec = curr.ec + dc[i];
            if (ner >= 0 && ner < N && nec >= 0 && nec < N) {
                if (locked[get_idx(ner, nec)] && !(ner == curr.tr && nec == curr.tc)) continue;

                int ntr = curr.tr, ntc = curr.tc;
                if (ner == curr.tr && nec == curr.tc) { ntr = curr.er; ntc = curr.ec; }

                int n_state_id = ((ntr * N + ntc) * N + ner) * N + nec;
                if (global_visited[n_state_id] != visit_token) {
                    int ng = curr.g + 1;
                    
                    int penalty = 0;
                    if (ntr == goal_r && ner == goal_r) {
                        if (goal_c > ntc && nec < ntc) penalty += 2;
                        if (goal_c < ntc && nec > ntc) penalty += 2;
                    }
                    if (ntc == goal_c && nec == goal_c) {
                        if (goal_r > ntr && ner < ntr) penalty += 2;
                        if (goal_r < ntr && ner > ntr) penalty += 2;
                    }

                    int nh = 5 * (abs(ntr - goal_r) + abs(ntc - goal_c)) + (abs(ntr - ner) + abs(ntc - nec)) + penalty;
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

string run_simulation(const vector<pair<int, int>>& target_order, int w_empty, int w_goal, int noise_level) {
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

        // 🔥 THE EARTHQUAKE: เขย่ากระดาน 1-3 ก้าวแบบสุ่มเพื่อหาทางลัดใหม่!
        if (noise_level > 0 && rand() % 100 < 10) { // โอกาส 10%
            int steps = rand() % 3 + 1;
            int dr[] = {1, -1, 0, 0};
            int dc[] = {0, 0, 1, -1};
            char move_char[] = {'U', 'D', 'L', 'R'};
            for (int k = 0; k < steps; ++k) {
                int d = rand() % 4;
                int ner = er + dr[d], nec = ec + dc[d];
                if (ner >= 0 && ner < N && nec >= 0 && nec < N && !locked[get_idx(ner, nec)]) {
                    swap(grid[get_idx(er, ec)], grid[get_idx(ner, nec)]);
                    er = ner; ec = nec;
                    final_answer += move_char[d];
                }
            }
        }

        vector<int> dist_from_goal = bfs_distances(i, j, locked);
        vector<int> dist_from_empty = bfs_distances(er, ec, locked);

        int best_tr = -1, best_tc = -1;
        int min_cost = 2e9;

        // 🔥 Schizophrenic AI: โอกาส 5% ที่จะเปลี่ยนนิสัยการหยิบเฉพาะแผ่นนี้
        int current_w_empty = w_empty;
        int current_w_goal = w_goal;
        if (noise_level > 0 && rand() % 100 < 5) {
            current_w_empty = rand() % 5 + 1;
            current_w_goal = rand() % 10 + 1;
        }

        for (int r = 0; r < N; ++r) {
            for (int c = 0; c < N; ++c) {
                if (grid[get_idx(r, c)] == goal_val && !locked[get_idx(r, c)]) {
                    if (dist_from_goal[get_idx(r, c)] == 1e9 || dist_from_empty[get_idx(r, c)] == 1e9) continue;

                    bool is_perfect_match = (r >= 1 && r <= N - 2 && c >= 1 && c <= N - 2 && 
                                             grid[get_idx(r, c)] == target_flat[(r - 1) * (N - 2) + (c - 1)]);
                    
                    int random_noise = (noise_level > 0) ? (rand() % (noise_level * 2)) : 0;
                    
                    int cost = (current_w_empty * dist_from_empty[get_idx(r, c)]) + 
                               (current_w_goal * dist_from_goal[get_idx(r, c)]) + 
                               random_noise + 
                               (is_perfect_match ? 10000 : 0);
                    
                    if (cost < min_cost) {
                        min_cost = cost; best_tr = r; best_tc = c;
                    }
                }
            }
        }

        if (best_tr == -1 || best_tc == -1) {
            return "FAILED"; // คืนค่า FAILED ไปเลย ให้มันไปสุ่มมั่วรูปแบบอื่นต่อ
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

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(false); cin.tie(NULL);

    // รับชื่อไฟล์ Backup จาก Terminal (ถ้าไม่ได้พิมพ์มา ให้ใช้ชื่อ default)
    string backup_filename = "best_solution_backup.txt";
    if (argc > 1) {
        backup_filename = string(argv[1]) + "_backup.txt"; 
    }

    if (!(cin >> N)) return 0;
    
    original_grid.assign(N * N, 0); 
    target_flat.assign((N - 2) * (N - 2), 0);
    global_visited.assign(N * N * N * N, 0); 

    for (int i = 0; i < N; ++i) for (int j = 0; j < N; ++j) cin >> original_grid[get_idx(i, j)];
    for (int i = 0; i < N - 2; ++i) for (int j = 0; j < N - 2; ++j) cin >> target_flat[i * (N - 2) + j];

    int M = N - 2;
    vector<pair<string, vector<pair<int, int>>>> strategies;

    // 1. Horizontal Snake
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
    strategies.push_back({"H-Snake (Top-Left)", h_tl});
    strategies.push_back({"H-Snake (Top-Right)", h_tr});
    strategies.push_back({"H-Snake (Bottom-Left)", h_bl});
    strategies.push_back({"H-Snake (Bottom-Right)", h_br});

    // 2. Vertical Snake
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
    strategies.push_back({"V-Snake (Top-Left)", v_tl});
    strategies.push_back({"V-Snake (Bottom-Left)", v_bl});
    strategies.push_back({"V-Snake (Top-Right)", v_tr});
    strategies.push_back({"V-Snake (Bottom-Right)", v_br});

    // 3. Fringe / L-Shape
    vector<pair<int, int>> f_tl_h, f_tl_v, f_tr_h, f_tr_v, f_bl_h, f_bl_v, f_br_h, f_br_v;
    for (int k = 1; k <= M; ++k) {
        int opp = M - k + 1; 
        for (int c = k; c <= M; ++c) f_tl_h.push_back({k, c});
        for (int r = k + 1; r <= M; ++r) f_tl_h.push_back({r, k});
        for (int r = k; r <= M; ++r) f_tl_v.push_back({r, k});
        for (int c = k + 1; c <= M; ++c) f_tl_v.push_back({k, c});
        for (int c = opp; c >= 1; --c) f_tr_h.push_back({k, c});
        for (int r = k + 1; r <= M; ++r) f_tr_h.push_back({r, opp});
        for (int r = k; r <= M; ++r) f_tr_v.push_back({r, opp});
        for (int c = opp - 1; c >= 1; --c) f_tr_v.push_back({k, c});
        for (int c = k; c <= M; ++c) f_bl_h.push_back({opp, c});
        for (int r = opp - 1; r >= 1; --r) f_bl_h.push_back({r, k});
        for (int r = opp; r >= 1; --r) f_bl_v.push_back({r, k});
        for (int c = k + 1; c <= M; ++c) f_bl_v.push_back({opp, c});
        for (int c = opp; c >= 1; --c) f_br_h.push_back({opp, c});
        for (int r = opp - 1; r >= 1; --r) f_br_h.push_back({r, opp});
        for (int r = opp; r >= 1; --r) f_br_v.push_back({r, opp});
        for (int c = opp - 1; c >= 1; --c) f_br_v.push_back({opp, c});
    }
    strategies.push_back({"Fringe (TL, Row-First)", f_tl_h});
    strategies.push_back({"Fringe (TL, Col-First)", f_tl_v});
    strategies.push_back({"Fringe (TR, Row-First)", f_tr_h});
    strategies.push_back({"Fringe (TR, Col-First)", f_tr_v});
    strategies.push_back({"Fringe (BL, Row-First)", f_bl_h});
    strategies.push_back({"Fringe (BL, Col-First)", f_bl_v});
    strategies.push_back({"Fringe (BR, Row-First)", f_br_h});
    strategies.push_back({"Fringe (BR, Col-First)", f_br_v});

    // 4. Diagonal Sweep
    vector<pair<int, int>> d_tl_1, d_tl_2, d_tr_1, d_tr_2, d_bl_1, d_bl_2, d_br_1, d_br_2;
    for (int s = 2; s <= 2 * M; ++s) {
        vector<pair<int, int>> curr_tl, curr_tr, curr_bl, curr_br;
        int min_r = max(1, s - M), max_r = min(M, s - 1);
        for (int r = min_r; r <= max_r; ++r) {
            int c = s - r;
            curr_tl.push_back({r, c}); curr_tr.push_back({r, M - c + 1});
            curr_bl.push_back({M - r + 1, c}); curr_br.push_back({M - r + 1, M - c + 1});
        }
        for (auto p : curr_tl) d_tl_1.push_back(p); for (auto p : curr_tr) d_tr_1.push_back(p);
        for (auto p : curr_bl) d_bl_1.push_back(p); for (auto p : curr_br) d_br_1.push_back(p);
        
        reverse(curr_tl.begin(), curr_tl.end()); for (auto p : curr_tl) d_tl_2.push_back(p);
        reverse(curr_tr.begin(), curr_tr.end()); for (auto p : curr_tr) d_tr_2.push_back(p);
        reverse(curr_bl.begin(), curr_bl.end()); for (auto p : curr_bl) d_bl_2.push_back(p);
        reverse(curr_br.begin(), curr_br.end()); for (auto p : curr_br) d_br_2.push_back(p);
    }
    strategies.push_back({"Diagonal (TL, Down-Left)", d_tl_1});
    strategies.push_back({"Diagonal (TL, Up-Right)", d_tl_2});
    strategies.push_back({"Diagonal (TR, Down-Right)", d_tr_1});
    strategies.push_back({"Diagonal (TR, Up-Left)", d_tr_2});
    strategies.push_back({"Diagonal (BL, Up-Left)", d_bl_1});
    strategies.push_back({"Diagonal (BL, Down-Right)", d_bl_2});
    strategies.push_back({"Diagonal (BR, Up-Right)", d_br_1});
    strategies.push_back({"Diagonal (BR, Down-Left)", d_br_2});

    // 5. Inside-Out Concentric Spiral (4 ทิศจบที่มุม)
    vector<vector<pair<int, int>>> layers;
    int top = 1, bottom = M, left = 1, right = M;
    while (top <= bottom && left <= right) {
        vector<pair<int, int>> layer;
        if (top == bottom && left == right) { 
            layer.push_back({top, left});
            layers.push_back(layer);
            break;
        }
        for (int c = left; c <= right; ++c) layer.push_back({top, c});
        for (int r = top + 1; r <= bottom; ++r) layer.push_back({r, right});
        if (top < bottom) {
            for (int c = right - 1; c >= left; --c) layer.push_back({bottom, c});
        }
        if (left < right) {
            for (int r = bottom - 1; r > top; --r) layer.push_back({r, left});
        }
        layers.push_back(layer);
        top++; bottom--; left++; right--;
    }
    
    reverse(layers.begin(), layers.end());
    vector<pair<int, int>> sp_tl, sp_tr, sp_br, sp_bl;
    
    for (const auto& l : layers) {
        if (l.size() <= 1) {
            for (auto p : l) { sp_tl.push_back(p); sp_tr.push_back(p); sp_br.push_back(p); sp_bl.push_back(p); }
            continue;
        }
        
        int c_top = 2e9, c_bottom = -1, c_left = 2e9, c_right = -1;
        for (auto p : l) {
            c_top = min(c_top, p.first); c_bottom = max(c_bottom, p.first);
            c_left = min(c_left, p.second); c_right = max(c_right, p.second);
        }
        
        int idx_tl = 0, idx_tr = 0, idx_br = 0, idx_bl = 0;
        for (int i = 0; i < l.size(); ++i) {
            if (l[i].first == c_top && l[i].second == c_left) idx_tl = i;
            if (l[i].first == c_top && l[i].second == c_right) idx_tr = i;
            if (l[i].first == c_bottom && l[i].second == c_right) idx_br = i;
            if (l[i].first == c_bottom && l[i].second == c_left) idx_bl = i;
        }
        
        auto add_rotated = [](vector<pair<int,int>>& target, const vector<pair<int,int>>& src, int end_idx) {
            int start_idx = (end_idx + 1) % src.size();
            for (int i = 0; i < src.size(); ++i) {
                target.push_back(src[(start_idx + i) % src.size()]);
            }
        };
        
        add_rotated(sp_tl, l, idx_tl); 
        add_rotated(sp_tr, l, idx_tr); 
        add_rotated(sp_br, l, idx_br); 
        add_rotated(sp_bl, l, idx_bl); 
    }
    
    strategies.push_back({"Inside-Out Spiral (End TL Corner)", sp_tl});
    strategies.push_back({"Inside-Out Spiral (End TR Corner)", sp_tr});
    strategies.push_back({"Inside-Out Spiral (End BR Corner)", sp_br});
    strategies.push_back({"Inside-Out Spiral (End BL Corner)", sp_bl});
    
    // 🔥 7 Personalities (Weights) 🔥
    vector<pair<int, int>> weights = {
        {1, 5},   // Balanced 
        {3, 5},   // Lazy Empty
        {1, 8},   // Greedy Goal
        {1, 10},  // Hyper Greedy Goal
        {1, 2},   // Slightly Goal Oriented
        {5, 2},   // Super Lazy Empty 
        {1, 1}    // Pure Shortest
    };

    srand(time(NULL));

    cerr << "[INFO] Running 196-Universe Baseline + Chaos Mode (Marathon Edition)...\n\n";

    string best_path = "";
    int min_len = 2e9;
    string best_name = "";

    // -------------------------------------------------------------
    // PHASE 1: กวาดพื้นฐาน 196 แบบ (ไม่มั่ว) เพื่อเซฟคะแนนขั้นต่ำ
    // -------------------------------------------------------------
    for (const auto& strategy : strategies) {
        for (const auto& w : weights) {
            string path = run_simulation(strategy.second, w.first, w.second, 0);
            if (path != "FAILED" && path.length() < min_len) {
                best_path = path;
                min_len = path.length();
                best_name = strategy.first + " [W: " + to_string(w.first) + ":" + to_string(w.second) + "]";
            }
        }
    }
    
    cerr << "[INFO] Phase 1 Complete. Best Pure Logic Move: " << min_len << " (" << best_name << ")\n";
    cerr << "[INFO] Entering PHASE 2: Chaos Mode for 24 HOURS...\n\n";

    // -------------------------------------------------------------
    // PHASE 2: โหมดมั่วปั่นคะแนน (พร้อม Progress Bar)
    // -------------------------------------------------------------
    time_t start_time = time(NULL);
    int iterations = 0;
    int time_limit_seconds = 86400; // ⏳ 86400 วินาที = 24 ชั่วโมง
    
    while (time(NULL) - start_time < time_limit_seconds) {
        iterations++;
        
        try { // <--- 🛡️ เพิ่มเกราะป้องกันตรงนี้
            // 1. สุ่มหยิบ 1 จักรวาล และ 1 น้ำหนัก
            int rand_strat_idx = rand() % strategies.size();
            int rand_weight_idx = rand() % weights.size();
            auto strategy = strategies[rand_strat_idx];
            auto w = weights[rand_weight_idx];
            
            // 2. มั่วคิวเป้าหมาย (Mutation)
            vector<pair<int, int>> mutated_order = strategy.second;
            if (mutated_order.size() > 3) {
                int mut_type = rand() % 3; 
                if (mut_type == 0) {
                    int swap_idx = rand() % (mutated_order.size() - 1);
                    swap(mutated_order[swap_idx], mutated_order[swap_idx + 1]);
                } else if (mut_type == 1) {
                    int idx1 = rand() % mutated_order.size();
                    int idx2 = rand() % mutated_order.size();
                    swap(mutated_order[idx1], mutated_order[idx2]);
                } else {
                    int idx1 = rand() % mutated_order.size();
                    int idx2 = rand() % mutated_order.size();
                    if (idx1 > idx2) swap(idx1, idx2);
                    reverse(mutated_order.begin() + idx1, mutated_order.begin() + idx2);
                }
            }
            
            // 3. มั่วตอนหยิบ (Noise)
            int noise = rand() % 5 + 1; 

            // 4. รัน
            string path = run_simulation(mutated_order, w.first, w.second, noise);
            
            // 📊 PROGRESS BAR
            if (iterations % 100 == 0) {
                int elapsed_seconds = time(NULL) - start_time;
                double percent = (double)elapsed_seconds / time_limit_seconds * 100.0;
                int h = elapsed_seconds / 3600;
                int m = (elapsed_seconds % 3600) / 60;
                int s = elapsed_seconds % 60;
                
                cerr << "\r[PROGRESS] " << fixed << setprecision(2) << percent << "% "
                     << "| Time: " << h << "h " << m << "m " << s << "s "
                     << "| Iterations: " << iterations 
                     << "| Current Best: " << min_len << " moves      " << flush;
            }
            
            // 🔥 ทำลายสถิติ! (New Record & Auto Backup)
            if (path != "FAILED" && path.length() < min_len) {
                best_path = path;
                min_len = path.length();
                best_name = "MUTATED " + strategy.first + " [W:" + to_string(w.first) + ":" + to_string(w.second) + ", Noise:" + to_string(noise) + "]";
                
                cerr << "\n\n🔥 NEW RECORD! Moves: " << min_len << " (" << best_name << ")\n";
                
                ofstream outfile(backup_filename); 
                if (outfile.is_open()) {
                    outfile << best_path.length() << "\n" << best_path << "S\n";
                    outfile.close();
                    cerr << "💾 Saved backup to '" << backup_filename << "'\n\n"; 
                }
            }
            
        } catch (const std::exception& e) {
            // 🚨 ถ้าเกิด RAM เต็ม หรือ Error มันจะพิมพ์บอก แทนที่จะดับหนี!
            cerr << "\n\n[WARNING] Caught Exception: " << e.what() << "\n";
            cerr << "Free up some RAM or close other heavy programs!\n\n";
        }
    }

    cerr << "\n\n[SUCCESS] Completed " << iterations << " random mutations in 24 Hours.\n";
    cerr << "The Winner is: " << best_name << " with " << best_path.length() << " moves!\n";
    
    cout << "\n-----------------------------------\n";
    cout << "Best Moves: " << best_path.length() << " (excluding 'S')\n";
    cout << best_path << "S\n";

    return 0;
}