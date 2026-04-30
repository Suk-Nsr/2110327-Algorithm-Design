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

// --- ตัวแปร Global ป้องกัน RAM ล้น ---
int N;
vector<int> original_grid;
vector<int> target_flat;
vector<unsigned long long> global_visited; // ใช้ ULL ป้องกัน Overflow ข้ามคืน
unsigned long long visit_token = 0;

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

// 🚀 เครื่องยนต์หลัก (รองรับ Earthquake และ Schizophrenic AI)
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

        // 🔥 ท่ามั่ว 1: The Earthquake (สุ่มเขย่ากระดาน 1-3 ก้าวเพื่อปลดล็อกซอยตัน)
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

        // 🔥 ท่ามั่ว 2: Schizophrenic AI (เปลี่ยนนิสัยกะทันหันกลางทาง)
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

        // 🛡️ ป้องกัน Segfault กรณีหากระเบื้องไม่เจอ (โดนบล็อคตาย)
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
    ios_base::sync_with_stdio(false); cin.tie(NULL);
    srand(time(NULL));

    // ระบบตั้งชื่อไฟล์ Auto Backup อัจฉริยะ
    string backup_filename;
    if (argc > 1) {
        backup_filename = string(argv[1]) + "_backup.txt"; 
    } else {
        backup_filename = "F2L_Chaos_Auto_" + to_string(time(NULL)) + "_" + to_string(rand() % 1000) + ".txt";
    }

    if (!(cin >> N)) return 0;
    
    original_grid.assign(N * N, 0); 
    target_flat.assign((N - 2) * (N - 2), 0);
    global_visited.assign(N * N * N * N, 0); 

    for (int i = 0; i < N; ++i) for (int j = 0; j < N; ++j) cin >> original_grid[get_idx(i, j)];
    for (int i = 0; i < N - 2; ++i) for (int j = 0; j < N - 2; ++j) cin >> target_flat[i * (N - 2) + j];

    int M = N - 2;
    vector<pair<string, vector<pair<int, int>>>> strategies;

    // 1-4: Snake & Fringe & Diagonal
    // สร้าง Horizontal Snake
    vector<pair<int, int>> h_tl;
    for (int i = 1; i <= M; ++i) {
        for (int step = 1; step <= M; ++step) h_tl.push_back({i, (i % 2 != 0) ? step : (M - step + 1)});
    }
    strategies.push_back({"Horizontal Snake", h_tl});

    // สร้าง Diagonal Sweep
    vector<pair<int, int>> d_tl;
    for (int s = 2; s <= 2 * M; ++s) {
        int min_r = max(1, s - M), max_r = min(M, s - 1);
        for (int r = min_r; r <= max_r; ++r) d_tl.push_back({r, s - r});
    }
    strategies.push_back({"Diagonal Sweep", d_tl});

    // 5. Inside-Out Concentric Spiral (แบบ Corner Finisher กันตาย)
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
    vector<pair<int, int>> sp_tl, sp_br;
    for (const auto& l : layers) {
        if (l.size() <= 1) { for (auto p : l) { sp_tl.push_back(p); sp_br.push_back(p); } continue; }
        int c_top = 2e9, c_bottom = -1, c_left = 2e9, c_right = -1;
        for (auto p : l) {
            c_top = min(c_top, p.first); c_bottom = max(c_bottom, p.first);
            c_left = min(c_left, p.second); c_right = max(c_right, p.second);
        }
        int idx_tl = 0, idx_br = 0;
        for (int i = 0; i < l.size(); ++i) {
            if (l[i].first == c_top && l[i].second == c_left) idx_tl = i;
            if (l[i].first == c_bottom && l[i].second == c_right) idx_br = i;
        }
        auto add_rotated = [](vector<pair<int,int>>& target, const vector<pair<int,int>>& src, int end_idx) {
            int start_idx = (end_idx + 1) % src.size();
            for (int i = 0; i < src.size(); ++i) target.push_back(src[(start_idx + i) % src.size()]);
        };
        add_rotated(sp_tl, l, idx_tl); 
        add_rotated(sp_br, l, idx_br); 
    }
    strategies.push_back({"Inside-Out Spiral (TL)", sp_tl});
    strategies.push_back({"Inside-Out Spiral (BR)", sp_br});

    vector<pair<int, int>> weights = {
        {1, 5}, {3, 5}, {1, 8}, {1, 10}, {1, 2}, {5, 2}, {1, 1}
    };

    cerr << "[INFO] Running Ultimate F2L Chaos Engine...\n\n";

    string best_path = "";
    int min_len = 2e9;
    string best_name = "";

    // PHASE 1: กวาดพื้นฐานเพื่อหา Baseline
    for (const auto& strategy : strategies) {
        for (const auto& w : weights) {
            string path = run_simulation(strategy.second, w.first, w.second, 0);
            if (path != "FAILED" && path.length() < min_len) {
                best_path = path; min_len = path.length();
                best_name = strategy.first;
            }
        }
    }
    
    cerr << "[INFO] Baseline Complete. Entering PHASE 2: F2L Chaos Mode...\n\n";

    // PHASE 2: F2L Chaos Mutation
    time_t start_time = time(NULL);
    int iterations = 0;
    int time_limit_seconds = 86400; // 24 ชั่วโมง
    
    while (true) {
        int elapsed_seconds = time(NULL) - start_time;
        if (elapsed_seconds >= time_limit_seconds) break;
        iterations++;
        
        try {
            int rand_strat_idx = rand() % strategies.size();
            int rand_weight_idx = rand() % weights.size();
            auto strategy = strategies[rand_strat_idx];
            auto w = weights[rand_weight_idx];
            
            vector<pair<int, int>> mutated_order = strategy.second;
            
            // 🔥 ท่ามั่ว 3: Deep DNA & F2L Block Mutation
            if (mutated_order.size() > 4) {
                int mut_type = rand() % 4; // เพิ่มเป็น 4 แบบ
                if (mut_type == 0) {
                    // สลับใกล้ชิด (Adjacent Swap)
                    int swap_idx = rand() % (mutated_order.size() - 1);
                    swap(mutated_order[swap_idx], mutated_order[swap_idx + 1]);
                } else if (mut_type == 1) {
                    // สลับข้ามโลก (Long-jump Swap)
                    int idx1 = rand() % mutated_order.size();
                    int idx2 = rand() % mutated_order.size();
                    swap(mutated_order[idx1], mutated_order[idx2]);
                } else if (mut_type == 2) {
                    // กลับด้านก้อน (2-Opt Inversion)
                    int idx1 = rand() % mutated_order.size();
                    int idx2 = rand() % mutated_order.size();
                    if (idx1 > idx2) swap(idx1, idx2);
                    reverse(mutated_order.begin() + idx1, mutated_order.begin() + idx2);
                } else {
                    // 🔥 ท่ามั่วลับ: F2L Block Swap (สลับกลุ่มทีละ 2-3 แผ่นพร้อมกัน!)
                    int block_size = (rand() % 2) + 2; // สุ่มบล็อกขนาด 2 หรือ 3 แผ่น
                    int max_idx = mutated_order.size() - block_size;
                    int idx1 = rand() % max_idx;
                    int idx2 = rand() % max_idx;
                    // ป้องกันบล็อกทับซ้อนกัน
                    if (abs(idx1 - idx2) >= block_size) {
                        for (int k = 0; k < block_size; ++k) {
                            swap(mutated_order[idx1 + k], mutated_order[idx2 + k]);
                        }
                    }
                }
            }
            
            int noise = rand() % 6; // ใส่ค่าเบลอ 0-5
            string path = run_simulation(mutated_order, w.first, w.second, noise);
            
            // 📊 Progress Bar
            if (iterations % 50 == 0) {
                double percent = (double)elapsed_seconds / time_limit_seconds * 100.0;
                int h = elapsed_seconds / 3600;
                int m = (elapsed_seconds % 3600) / 60;
                int s = elapsed_seconds % 60;
                cerr << "\r[PROGRESS] " << fixed << setprecision(2) << percent << "% "
                     << "| Time: " << h << "h " << m << "m " << s << "s "
                     << "| Iters: " << iterations 
                     << "| Best: " << min_len << " moves      " << flush;
            }
            
            // 💾 ทำลายสถิติ!
            if (path != "FAILED" && path.length() < min_len) {
                best_path = path;
                min_len = path.length();
                best_name = "F2L-MUTATED " + strategy.first + " [Noise:" + to_string(noise) + "]";
                
                cerr << "\n\n🔥 NEW RECORD! Moves: " << min_len << " (" << best_name << ")\n";
                
                ofstream outfile(backup_filename); 
                if (outfile.is_open()) {
                    outfile << best_path.length() << "\n" << best_path << "S\n";
                    outfile.close();
                    cerr << "💾 Saved backup to '" << backup_filename << "'\n\n"; 
                }
            }
            
        } catch (const std::exception& e) {
            cerr << "\n[WARNING] Exception: " << e.what() << "\n";
        }
    }

    cerr << "\n\n[SUCCESS] Completed " << iterations << " random mutations in 24 Hours.\n";
    cerr << "The Winner is: " << best_name << " with " << best_path.length() << " moves!\n";
    
    cout << "\n-----------------------------------\n";
    cout << "Best Moves: " << best_path.length() << " (excluding 'S')\n";
    cout << best_path << "S\n";

    return 0;
}