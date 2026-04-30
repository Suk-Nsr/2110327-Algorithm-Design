#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <cmath>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <fstream>

using namespace std;

int N;
vector<int> original_grid;
vector<int> target_flat;
vector<unsigned long long> global_visited; 
unsigned long long visit_token = 0; // ป้องกัน Overflow ข้ามคืน

inline int get_idx(int r, int c) { return r * N + c; }

struct Node {
    int tr, tc, er, ec, g, f;
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
                    
                    // 🔥 Heuristic 1: Hovering Penalty (กันช่องว่างยืนเกะกะ)
                    if (goal_c > ntc && nec < ntc) penalty += 5; 
                    if (goal_c < ntc && nec > ntc) penalty += 5; 
                    if (goal_r > ntr && ner < ntr) penalty += 5; 
                    if (goal_r < ntr && ner > ntr) penalty += 5; 

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
    for (int i = 0; i < N * N; ++i) {
        if (grid[i] == -1) { er = i / N; ec = i % N; break; }
    }

    string final_answer = "";

    // 🚨 ใช้ Target Order ตามลำดับคิวเดิม ห้ามแหกคิวเด็ดขาด!
    for (auto goal_pos : target_order) {
        int i = goal_pos.first;
        int j = goal_pos.second;
        int goal_val = target_flat[(i - 1) * (N - 2) + (j - 1)];

        if (grid[get_idx(i, j)] == goal_val) {
            locked[get_idx(i, j)] = true;
            continue;
        }

        if (noise_level > 0 && rand() % 100 < 10) {
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
                    
                    // 🔥 Heuristic 2: Crowding Penalty (รังเกียจซอกหลืบ)
                    int crowded_penalty = 0;
                    int ddr[] = {1, -1, 0, 0}, ddc[] = {0, 0, 1, -1};
                    for(int d=0; d<4; ++d) {
                        int nr = r + ddr[d], nc = c + ddc[d];
                        if(nr >= 0 && nr < N && nc >= 0 && nc < N) {
                            if(locked[get_idx(nr, nc)]) crowded_penalty += 15; 
                        } else crowded_penalty += 15; 
                    }

                    int random_noise = (noise_level > 0) ? (rand() % (noise_level * 2)) : 0;
                    int cost = (current_w_empty * dist_from_empty[get_idx(r, c)]) + 
                               (current_w_goal * dist_from_goal[get_idx(r, c)]) + 
                               crowded_penalty + random_noise + 
                               (is_perfect_match ? 10000 : 0);
                    
                    if (cost < min_cost) {
                        min_cost = cost; best_tr = r; best_tc = c;
                    }
                }
            }
        }

        if (best_tr == -1 || best_tc == -1) return "FAILED";
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
    ios_base::sync_with_stdio(false); cin.tie(NULL); srand(time(NULL));

    string backup_filename;
    if (argc > 1) backup_filename = string(argv[1]) + "_backup.txt"; 
    else backup_filename = "Safe_MicroOpt_Auto_" + to_string(time(NULL)) + ".txt";

    if (!(cin >> N)) return 0;
    
    original_grid.assign(N * N, 0); 
    target_flat.assign((N - 2) * (N - 2), 0);
    global_visited.assign(N * N * N * N, 0); 

    for (int i = 0; i < N; ++i) for (int j = 0; j < N; ++j) cin >> original_grid[get_idx(i, j)];
    for (int i = 0; i < N - 2; ++i) for (int j = 0; j < N - 2; ++j) cin >> target_flat[i * (N - 2) + j];

    int M = N - 2;
    vector<pair<string, vector<pair<int, int>>>> strategies;

    // --- เพิ่ม Pattern แบบดั้งเดิมของคุณทั้งหมดตรงนี้ (Snake, Spiral ฯลฯ) ---
    vector<pair<int, int>> h_tl;
    for (int i = 1; i <= M; ++i) {
        for (int step = 1; step <= M; ++step) h_tl.push_back({i, (i % 2 != 0) ? step : (M - step + 1)});
    }
    strategies.push_back({"Horizontal Snake", h_tl});

    vector<vector<pair<int, int>>> layers;
    int top = 1, bottom = M, left = 1, right = M;
    while (top <= bottom && left <= right) {
        vector<pair<int, int>> layer;
        if (top == bottom && left == right) { layer.push_back({top, left}); layers.push_back(layer); break; }
        for (int c = left; c <= right; ++c) layer.push_back({top, c});
        for (int r = top + 1; r <= bottom; ++r) layer.push_back({r, right});
        if (top < bottom) for (int c = right - 1; c >= left; --c) layer.push_back({bottom, c});
        if (left < right) for (int r = bottom - 1; r > top; --r) layer.push_back({r, left});
        layers.push_back(layer);
        top++; bottom--; left++; right--;
    }
    reverse(layers.begin(), layers.end());
    vector<pair<int, int>> sp_tl;
    for (const auto& l : layers) {
        if (l.size() <= 1) { for (auto p : l) sp_tl.push_back(p); continue; }
        int c_top = 2e9, c_left = 2e9;
        for (auto p : l) { c_top = min(c_top, p.first); c_left = min(c_left, p.second); }
        int idx_tl = 0;
        for (int i = 0; i < l.size(); ++i) if (l[i].first == c_top && l[i].second == c_left) idx_tl = i;
        int start_idx = (idx_tl + 1) % l.size();
        for (int i = 0; i < l.size(); ++i) sp_tl.push_back(l[(start_idx + i) % l.size()]);
    }
    strategies.push_back({"Inside-Out Spiral", sp_tl});

    vector<pair<int, int>> weights = {{1, 5}, {3, 5}, {1, 8}, {1, 10}, {1, 2}, {5, 2}, {1, 1}};

    cerr << "[INFO] Running SAFE Micro-Optimized Engine...\n\n";

    string best_path = "";
    int min_len = 2e9;
    string best_name = "";
    
    time_t start_time = time(NULL);
    time_t last_print_time = 0;
    int iterations = 0;

    while (time(NULL) - start_time < 86400) {
        iterations++;
        
        try {
            int rand_strat_idx = rand() % strategies.size();
            int rand_weight_idx = rand() % weights.size();
            auto strategy = strategies[rand_strat_idx];
            auto w = weights[rand_weight_idx];
            vector<pair<int, int>> mutated_order = strategy.second;
            
            if (mutated_order.size() > 4) {
                int mut_type = rand() % 4; 
                if (mut_type == 0) {
                    int swap_idx = rand() % (mutated_order.size() - 1);
                    swap(mutated_order[swap_idx], mutated_order[swap_idx + 1]);
                } else if (mut_type == 1) {
                    int idx1 = rand() % mutated_order.size(), idx2 = rand() % mutated_order.size();
                    swap(mutated_order[idx1], mutated_order[idx2]);
                } else if (mut_type == 2) {
                    int idx1 = rand() % mutated_order.size(), idx2 = rand() % mutated_order.size();
                    if (idx1 > idx2) swap(idx1, idx2);
                    reverse(mutated_order.begin() + idx1, mutated_order.begin() + idx2);
                } else {
                    // 🔥 Heuristic 3: Block Swap Mutation (สลับเป็นก้อน)
                    int block_size = (rand() % 3) + 2; 
                    int max_idx = mutated_order.size() - block_size;
                    int idx1 = rand() % max_idx, idx2 = rand() % max_idx;
                    if (abs(idx1 - idx2) >= block_size) {
                        for (int k = 0; k < block_size; ++k) swap(mutated_order[idx1 + k], mutated_order[idx2 + k]);
                    }
                }
            }
            
            int noise = rand() % 6; 
            string path = run_simulation(mutated_order, w.first, w.second, noise);
            
            time_t current_time = time(NULL);
            if (current_time - last_print_time >= 1) {
                last_print_time = current_time;
                int elapsed = current_time - start_time;
                cerr << "\r[PROGRESS] Time: " << elapsed/3600 << "h " << (elapsed%3600)/60 << "m " << elapsed%60 << "s "
                     << "| Iters: " << iterations 
                     << "| Best: " << (min_len == 2e9 ? 0 : min_len) << " moves      " << flush;
            }
            
            if (path != "FAILED" && path.length() < min_len) {
                best_path = path;
                min_len = path.length();
                best_name = "MUTATED " + strategy.first + " [Noise:" + to_string(noise) + "]";
                
                cerr << "\n\n🔥 NEW SAFE-OPT RECORD! Moves: " << min_len << " (" << best_name << ")\n";
                
                ofstream outfile(backup_filename); 
                if (outfile.is_open()) {
                    outfile << best_path.length() << "\n" << best_path << "S\n";
                    outfile.close();
                }
            }
            
        } catch (const std::exception& e) {
            cerr << "\n[WARNING] Exception: " << e.what() << "\n";
        }
    }
    return 0;
}