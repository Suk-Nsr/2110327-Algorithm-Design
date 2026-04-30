#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <cmath>
#include <algorithm>

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

// --- F2L Task Structure ---
struct Task {
    bool is_f2l;
    int r1, c1; // Target 1 (or single target)
    int r2, c2; // Target 2
    int sr1, sc1, sr2, sc2; // Staging locations for F2L
    int er, ec; // Where empty space needs to be
    string f2l_moves; // The 2-move F2L finishing combo
};

// ฟังก์ชันจำลองเส้นทาง (หลบกำแพง)
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

// อัปเกรด A* ให้มี Linear Conflict (+ penalty)
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

// ระบบพาช่องว่างวิ่งไปรอ (BFS เฉพาะช่องว่าง)
string route_empty(int start_er, int start_ec, int goal_er, int goal_ec, const vector<bool>& locked) {
    if (start_er == goal_er && start_ec == goal_ec) return "";
    queue<pair<int, string>> q;
    vector<bool> vis(N * N, false);
    q.push({get_idx(start_er, start_ec), ""});
    vis[get_idx(start_er, start_ec)] = true;
    
    int dr[] = {1, -1, 0, 0}, dc[] = {0, 0, 1, -1};
    char move_char[] = {'U', 'D', 'L', 'R'}; 
    
    while (!q.empty()) {
        auto [idx, path] = q.front(); q.pop();
        int r = idx / N, c = idx % N;
        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i], nc = c + dc[i];
            if (nr >= 0 && nr < N && nc >= 0 && nc < N && !locked[get_idx(nr, nc)]) {
                if (nr == goal_er && nc == goal_ec) return path + move_char[i];
                if (!vis[get_idx(nr, nc)]) {
                    vis[get_idx(nr, nc)] = true;
                    q.push({get_idx(nr, nc), path + move_char[i]});
                }
            }
        }
    }
    return "FAILED";
}

void apply_path(const string& path, int& er, int& ec, vector<int>& grid) {
    for (char m : path) {
        int ner = er, nec = ec;
        if (m == 'U') ner++; else if (m == 'D') ner--; else if (m == 'L') nec++; else if (m == 'R') nec--;
        swap(grid[get_idx(er, ec)], grid[get_idx(ner, nec)]);
        er = ner; ec = nec;
    }
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

// ---------------------------------------------------------
// 🔥 THE F2L GENERATOR ENGINE 🔥
// ---------------------------------------------------------
void add_line(vector<Task>& tasks, const vector<pair<int,int>>& line, int free_dr, int free_dc) {
    if (line.size() < 2) {
        for (auto p : line) tasks.push_back({false, p.first, p.second, 0,0,0,0,0,0,0,0,""});
        return;
    }
    for (int i = 0; i < (int)line.size() - 2; ++i) {
        tasks.push_back({false, line[i].first, line[i].second, 0,0,0,0,0,0,0,0,""});
    }
    int r1 = line[line.size()-2].first, c1 = line[line.size()-2].second;
    int r2 = line[line.size()-1].first, c2 = line[line.size()-1].second;
    
    int sr2 = r1, sc2 = c1;
    int sr1 = r1 + free_dr, sc1 = c1 + free_dc;
    int er = r2, ec = c2;
    
    int dtr1 = r2 - r1, dtc1 = c2 - c1;
    char m1 = (dtr1 == 1) ? 'D' : (dtr1 == -1) ? 'U' : (dtc1 == 1) ? 'R' : 'L';
    
    int dtr2 = -free_dr, dtc2 = -free_dc;
    char m2 = (dtr2 == 1) ? 'D' : (dtr2 == -1) ? 'U' : (dtc2 == 1) ? 'R' : 'L';
    
    string moves = ""; moves += m1; moves += m2;
    tasks.push_back({true, r1, c1, r2, c2, sr1, sc1, sr2, sc2, er, ec, moves});
}

string run_simulation(const vector<Task>& tasks) {
    vector<int> grid = original_grid;
    vector<bool> locked(N * N, false);
    int er = -1, ec = -1;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) { if (grid[get_idx(i, j)] == -1) { er = i; ec = j; } }
    }

    string final_answer = "";

    auto solve_tile = [&](int goal_val, int goal_r, int goal_c) {
        if (grid[get_idx(goal_r, goal_c)] == goal_val) return string("");
        vector<int> dist_from_goal = bfs_distances(goal_r, goal_c, locked);
        vector<int> dist_from_empty = bfs_distances(er, ec, locked);
        int best_tr = -1, best_tc = -1, min_cost = 2e9;
        for (int r = 0; r < N; ++r) {
            for (int c = 0; c < N; ++c) {
                if (grid[get_idx(r, c)] == goal_val && !locked[get_idx(r, c)]) {
                    if (dist_from_goal[get_idx(r, c)] == 1e9 || dist_from_empty[get_idx(r, c)] == 1e9) continue;
                    bool is_perf = (r >= 1 && r <= N-2 && c >= 1 && c <= N-2 && grid[get_idx(r, c)] == target_flat[(r-1)*(N-2)+(c-1)]);
                    int cost = dist_from_empty[get_idx(r, c)] + 5 * dist_from_goal[get_idx(r, c)] + (is_perf ? 10000 : 0);
                    if (cost < min_cost) { min_cost = cost; best_tr = r; best_tc = c; }
                }
            }
        }
        if (best_tr == -1) return string("FAILED");
        return solve_single_tile(best_tr, best_tc, er, ec, goal_r, goal_c, locked);
    };

    for (const auto& task : tasks) {
        if (!task.is_f2l) {
            int goal_val = target_flat[(task.r1 - 1) * (N - 2) + (task.c1 - 1)];
            if (grid[get_idx(task.r1, task.c1)] == goal_val) {
                locked[get_idx(task.r1, task.c1)] = true; continue;
            }
            string p = solve_tile(goal_val, task.r1, task.c1);
            if (p == "FAILED") return "FAILED";
            apply_path(p, er, ec, grid);
            final_answer += p;
            locked[get_idx(task.r1, task.c1)] = true;
        } else {
            int val1 = target_flat[(task.r1 - 1) * (N - 2) + (task.c1 - 1)];
            int val2 = target_flat[(task.r2 - 1) * (N - 2) + (task.c2 - 1)];
            
            if (grid[get_idx(task.r1, task.c1)] == val1 && grid[get_idx(task.r2, task.c2)] == val2) {
                locked[get_idx(task.r1, task.c1)] = true; locked[get_idx(task.r2, task.c2)] = true; continue;
            }

            string p2 = solve_tile(val2, task.sr2, task.sc2);
            if (p2 == "FAILED") return "FAILED";
            apply_path(p2, er, ec, grid); final_answer += p2;
            locked[get_idx(task.sr2, task.sc2)] = true;

            string p1 = solve_tile(val1, task.sr1, task.sc1);
            if (p1 == "FAILED") return "FAILED";
            apply_path(p1, er, ec, grid); final_answer += p1;
            locked[get_idx(task.sr1, task.sc1)] = true;

            string emp = route_empty(er, ec, task.er, task.ec, locked);
            if (emp == "FAILED") return "FAILED";
            apply_path(emp, er, ec, grid); final_answer += emp;

            locked[get_idx(task.sr2, task.sc2)] = false; locked[get_idx(task.sr1, task.sc1)] = false;
            apply_path(task.f2l_moves, er, ec, grid); final_answer += task.f2l_moves;

            locked[get_idx(task.r1, task.c1)] = true; locked[get_idx(task.r2, task.c2)] = true;
        }
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
    vector<pair<string, vector<Task>>> strategies;

    // 1. Horizontal Snake (4 ทิศ)
    vector<Task> h_tl, h_tr, h_bl, h_br;
    for (int i = 1; i <= M; ++i) {
        vector<pair<int, int>> l1, l2;
        if (i % 2 != 0) {
            for (int c = 1; c <= M; ++c) l1.push_back({i, c});
            for (int c = M; c >= 1; --c) l2.push_back({i, c});
        } else {
            for (int c = M; c >= 1; --c) l1.push_back({i, c});
            for (int c = 1; c <= M; ++c) l2.push_back({i, c});
        }
        add_line(h_tl, l1, 1, 0); add_line(h_tr, l2, 1, 0);
    }
    for (int i = M, row = 1; i >= 1; --i, ++row) {
        vector<pair<int, int>> l1, l2;
        if (row % 2 != 0) {
            for (int c = 1; c <= M; ++c) l1.push_back({i, c});
            for (int c = M; c >= 1; --c) l2.push_back({i, c});
        } else {
            for (int c = M; c >= 1; --c) l1.push_back({i, c});
            for (int c = 1; c <= M; ++c) l2.push_back({i, c});
        }
        add_line(h_bl, l1, -1, 0); add_line(h_br, l2, -1, 0);
    }
    strategies.push_back({"H-Snake (Top-Left)", h_tl}); strategies.push_back({"H-Snake (Top-Right)", h_tr});
    strategies.push_back({"H-Snake (Bottom-Left)", h_bl}); strategies.push_back({"H-Snake (Bottom-Right)", h_br});

    // 2. Vertical Snake (4 ทิศ)
    vector<Task> v_tl, v_tr, v_bl, v_br;
    for (int j = 1; j <= M; ++j) {
        vector<pair<int, int>> l1, l2;
        if (j % 2 != 0) {
            for (int r = 1; r <= M; ++r) l1.push_back({r, j});
            for (int r = M; r >= 1; --r) l2.push_back({r, j});
        } else {
            for (int r = M; r >= 1; --r) l1.push_back({r, j});
            for (int r = 1; r <= M; ++r) l2.push_back({r, j});
        }
        add_line(v_tl, l1, 0, 1); add_line(v_bl, l2, 0, 1);
    }
    for (int j = M, col = 1; j >= 1; --j, ++col) {
        vector<pair<int, int>> l1, l2;
        if (col % 2 != 0) {
            for (int r = 1; r <= M; ++r) l1.push_back({r, j});
            for (int r = M; r >= 1; --r) l2.push_back({r, j});
        } else {
            for (int r = M; r >= 1; --r) l1.push_back({r, j});
            for (int r = 1; r <= M; ++r) l2.push_back({r, j});
        }
        add_line(v_tr, l1, 0, -1); add_line(v_br, l2, 0, -1);
    }
    strategies.push_back({"V-Snake (Top-Left)", v_tl}); strategies.push_back({"V-Snake (Bottom-Left)", v_bl});
    strategies.push_back({"V-Snake (Top-Right)", v_tr}); strategies.push_back({"V-Snake (Bottom-Right)", v_br});

    // ========================================================
    // 3. Fringe / L-Shape (8 ทิศทาง - Row First & Col First)
    // ========================================================
    vector<Task> f_tl_h, f_tl_v, f_tr_h, f_tr_v, f_bl_h, f_bl_v, f_br_h, f_br_v;
    for (int k = 1; k <= M; ++k) {
        int opp = M - k + 1; 

        // Top-Left (Row-First)
        vector<pair<int, int>> r_tl_h, c_tl_h;
        for (int c = k; c <= M; ++c) r_tl_h.push_back({k, c});
        for (int r = k + 1; r <= M; ++r) c_tl_h.push_back({r, k});
        add_line(f_tl_h, r_tl_h, 1, 0); add_line(f_tl_h, c_tl_h, 0, 1);

        // Top-Left (Col-First)
        vector<pair<int, int>> c_tl_v, r_tl_v;
        for (int r = k; r <= M; ++r) c_tl_v.push_back({r, k});
        for (int c = k + 1; c <= M; ++c) r_tl_v.push_back({k, c});
        add_line(f_tl_v, c_tl_v, 0, 1); add_line(f_tl_v, r_tl_v, 1, 0);

        // Top-Right (Row-First)
        vector<pair<int, int>> r_tr_h, c_tr_h;
        for (int c = opp; c >= 1; --c) r_tr_h.push_back({k, c});
        for (int r = k + 1; r <= M; ++r) c_tr_h.push_back({r, opp});
        add_line(f_tr_h, r_tr_h, 1, 0); add_line(f_tr_h, c_tr_h, 0, -1);

        // Top-Right (Col-First)
        vector<pair<int, int>> c_tr_v, r_tr_v;
        for (int r = k; r <= M; ++r) c_tr_v.push_back({r, opp});
        for (int c = opp - 1; c >= 1; --c) r_tr_v.push_back({k, c});
        add_line(f_tr_v, c_tr_v, 0, -1); add_line(f_tr_v, r_tr_v, 1, 0);

        // Bottom-Left (Row-First)
        vector<pair<int, int>> r_bl_h, c_bl_h;
        for (int c = k; c <= M; ++c) r_bl_h.push_back({opp, c});
        for (int r = opp - 1; r >= 1; --r) c_bl_h.push_back({r, k});
        add_line(f_bl_h, r_bl_h, -1, 0); add_line(f_bl_h, c_bl_h, 0, 1);

        // Bottom-Left (Col-First)
        vector<pair<int, int>> c_bl_v, r_bl_v;
        for (int r = opp; r >= 1; --r) c_bl_v.push_back({r, k});
        for (int c = k + 1; c <= M; ++c) r_bl_v.push_back({opp, c});
        add_line(f_bl_v, c_bl_v, 0, 1); add_line(f_bl_v, r_bl_v, -1, 0);

        // Bottom-Right (Row-First)
        vector<pair<int, int>> r_br_h, c_br_h;
        for (int c = opp; c >= 1; --c) r_br_h.push_back({opp, c});
        for (int r = opp - 1; r >= 1; --r) c_br_h.push_back({r, opp});
        add_line(f_br_h, r_br_h, -1, 0); add_line(f_br_h, c_br_h, 0, -1);

        // Bottom-Right (Col-First)
        vector<pair<int, int>> c_br_v, r_br_v;
        for (int r = opp; r >= 1; --r) c_br_v.push_back({r, opp});
        for (int c = opp - 1; c >= 1; --c) r_br_v.push_back({opp, c});
        add_line(f_br_v, c_br_v, 0, -1); add_line(f_br_v, r_br_v, -1, 0);
    }
    strategies.push_back({"Fringe (Top-Left, Row-First)", f_tl_h});
    strategies.push_back({"Fringe (Top-Left, Col-First)", f_tl_v});
    strategies.push_back({"Fringe (Top-Right, Row-First)", f_tr_h});
    strategies.push_back({"Fringe (Top-Right, Col-First)", f_tr_v});
    strategies.push_back({"Fringe (Bottom-Left, Row-First)", f_bl_h});
    strategies.push_back({"Fringe (Bottom-Left, Col-First)", f_bl_v});
    strategies.push_back({"Fringe (Bottom-Right, Row-First)", f_br_h});
    strategies.push_back({"Fringe (Bottom-Right, Col-First)", f_br_v});

    // 🔥 BATTLE ROYALE (16 Universes) 🔥
    cerr << "[INFO] Running F2L-Powered 16-Universe Ensemble...\n\n";
    string best_path = "", best_name = ""; int min_len = 2e9;

    for (const auto& strategy : strategies) {
        cerr << "Running " << strategy.first << "... ";
        string path = run_simulation(strategy.second);
        if (path != "FAILED") {
            cerr << path.length() << " moves\n";
            if (path.length() < min_len) {
                best_path = path; min_len = path.length(); best_name = strategy.first;
            }
        } else cerr << "FAILED\n";
    }

    cerr << "\n[SUCCESS] The Winner is: " << best_name << " with " << best_path.length() << " moves!\n";
    cout << "\n-----------------------------------\n";
    cout << "Best Moves: " << best_path.length() << " (excluding 'S')\n" << best_path << "S\n";

    return 0;
}