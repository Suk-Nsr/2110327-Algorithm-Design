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
#include <unordered_set>
#include <cstdint>

using namespace std;

// --- ตัวแปร Global ---
int N;
vector<int> original_grid;
vector<int> target_flat;
vector<unsigned long long> global_visited; 
unsigned long long visit_token = 0;

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

// 🔥 ท่าประหลาด 2: ตาทิพย์ A* (Hovering Penalty)
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
                    int manhattan = (abs(ntr - goal_r) + abs(ntc - goal_c));
                    int empty_travel = (abs(ntr - ner) + abs(ntc - nec));
                    int nh = 5 * manhattan + empty_travel;

                    // Hovering Penalty: สอนให้ช่องว่างดักรอให้ถูกฝั่ง
                    if (goal_c > ntc && nec < ntc) nh += 6; 
                    if (goal_c < ntc && nec > ntc) nh += 6; 
                    if (goal_r > ntr && ner < ntr) nh += 6; 
                    if (goal_r < ntr && ner > ntr) nh += 6; 

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

struct SimResult {
    string path;
    vector<int> grid;
    vector<bool> locked;
    int er, ec;
    bool success;
};

// 🎯 The IDA* Finisher (หาทางที่ดีที่สุด 100% สำหรับ 8 แผ่นสุดท้าย)
string solve_perfect_endgame(vector<int> grid, vector<bool> locked, int er, int ec, const vector<pair<int, int>>& targets) {
    struct EndNode {
        vector<int> grid; int er, ec, g, h; string path;
        bool operator>(const EndNode& o) const { return (g + h) > (o.g + o.h); }
    };
    
    auto calc_h = [&](const vector<int>& g) {
        int h = 0;
        for (auto tg : targets) {
            int goal_val = target_flat[(tg.first - 1) * (N - 2) + (tg.second - 1)];
            for (int i = 0; i < N * N; ++i) {
                if (g[i] == goal_val) {
                    h += abs((i / N) - tg.first) + abs((i % N) - tg.second);
                    break;
                }
            }
        }
        return h;
    };

    priority_queue<EndNode, vector<EndNode>, greater<EndNode>> pq;
    unordered_set<uint64_t> visited;
    
    pq.push({grid, er, ec, 0, calc_h(grid), ""});
    
    int dr[] = {1, -1, 0, 0};
    int dc[] = {0, 0, 1, -1};
    char move_char[] = {'U', 'D', 'L', 'R'};

    int safety_limit = 200000; // ป้องกันค้าง
    while (!pq.empty() && safety_limit-- > 0) {
        auto curr = pq.top(); pq.pop();
        if (curr.h == 0) return curr.path; // จัดเรียงครบแล้ว!

        uint64_t hash = curr.er * N + curr.ec;
        for (int i = 0; i < N * N; ++i) {
            if (!locked[i] && curr.grid[i] > 0) hash ^= (curr.grid[i] * 1999 + i * 1009);
        }
        if (visited.count(hash)) continue;
        visited.insert(hash);

        for (int i = 0; i < 4; ++i) {
            int ner = curr.er + dr[i], nec = curr.ec + dc[i];
            if (ner >= 0 && ner < N && nec >= 0 && nec < N && !locked[get_idx(ner, nec)]) {
                vector<int> ngrid = curr.grid;
                swap(ngrid[get_idx(curr.er, curr.ec)], ngrid[get_idx(ner, nec)]);
                int nh = calc_h(ngrid);
                pq.push({ngrid, ner, nec, curr.g + 1, nh, curr.path + move_char[i]});
            }
        }
    }
    return "FAILED";
}

// 🚀 รัน Staged Beam สำหรับช่วงปกติ
SimResult run_partial_simulation(
    const vector<pair<int, int>>& target_order, int w_empty, int w_goal, int noise_level,
    vector<int> grid, vector<bool> locked, int er, int ec) 
{
    SimResult res; res.path = "";
    
    // ถ้าเป้าหมายเหลือน้อยกว่า 8 แผ่น โยนให้ Finisher จัดการ!
    if (target_order.size() <= 8 && noise_level == 0) {
        string perfect_path = solve_perfect_endgame(grid, locked, er, ec, target_order);
        if (perfect_path != "FAILED") {
            for (char m : perfect_path) {
                int ner = er, nec = ec;
                if (m == 'U') ner++; else if (m == 'D') ner--; else if (m == 'L') nec++; else if (m == 'R') nec--;
                swap(grid[get_idx(er, ec)], grid[get_idx(ner, nec)]);
                er = ner; ec = nec;
            }
            res.path = perfect_path;
            res.grid = grid; res.locked = locked; res.er = er; res.ec = ec; res.success = true;
            return res;
        }
    }

    for (auto goal_pos : target_order) {
        int i = goal_pos.first, j = goal_pos.second;
        int goal_val = target_flat[(i - 1) * (N - 2) + (j - 1)];

        if (grid[get_idx(i, j)] == goal_val) { locked[get_idx(i, j)] = true; continue; }

        if (noise_level > 0 && rand() % 100 < 10) {
            int steps = rand() % 3 + 1;
            int dr[] = {1, -1, 0, 0}, dc[] = {0, 0, 1, -1};
            char mc[] = {'U', 'D', 'L', 'R'};
            for (int k = 0; k < steps; ++k) {
                int d = rand() % 4;
                int ner = er + dr[d], nec = ec + dc[d];
                if (ner >= 0 && ner < N && nec >= 0 && nec < N && !locked[get_idx(ner, nec)]) {
                    swap(grid[get_idx(er, ec)], grid[get_idx(ner, nec)]);
                    er = ner; ec = nec; res.path += mc[d];
                }
            }
        }

        vector<int> dist_from_goal = bfs_distances(i, j, locked);
        vector<int> dist_from_empty = bfs_distances(er, ec, locked);
        int best_tr = -1, best_tc = -1, min_cost = 2e9;

        for (int r = 0; r < N; ++r) {
            for (int c = 0; c < N; ++c) {
                if (grid[get_idx(r, c)] == goal_val && !locked[get_idx(r, c)]) {
                    if (dist_from_goal[get_idx(r, c)] == 1e9 || dist_from_empty[get_idx(r, c)] == 1e9) continue;
                    
                    // 🔥 ท่าประหลาด 1: รังเกียจฝูงชน (เลี่ยงหยิบกระเบื้องติดกำแพง)
                    int crowded_penalty = 0;
                    int ddr[] = {1, -1, 0, 0}, ddc[] = {0, 0, 1, -1};
                    for(int d=0; d<4; ++d) {
                        int nr = r + ddr[d], nc = c + ddc[d];
                        if(nr >= 0 && nr < N && nc >= 0 && nc < N) {
                            if(locked[get_idx(nr, nc)]) crowded_penalty += 30; 
                        } else crowded_penalty += 30; 
                    }

                    int cost = (w_empty * dist_from_empty[get_idx(r, c)]) + 
                               (w_goal * dist_from_goal[get_idx(r, c)]) + crowded_penalty + 
                               ((noise_level > 0) ? (rand() % (noise_level * 2)) : 0);
                    
                    if (cost < min_cost) { min_cost = cost; best_tr = r; best_tc = c; }
                }
            }
        }

        if (best_tr == -1 || best_tc == -1) { res.success = false; return res; }
        string path_part = solve_single_tile(best_tr, best_tc, er, ec, i, j, locked);
        if (path_part == "FAILED") { res.success = false; return res; }

        for (char m : path_part) {
            int ner = er, nec = ec;
            if (m == 'U') ner++; else if (m == 'D') ner--; else if (m == 'L') nec++; else if (m == 'R') nec--;
            swap(grid[get_idx(er, ec)], grid[get_idx(ner, nec)]);
            er = ner; ec = nec;
        }
        res.path += path_part;
        locked[get_idx(i, j)] = true;
    }
    
    res.grid = grid; res.locked = locked; res.er = er; res.ec = ec; res.success = true;
    res.path = optimize_path(res.path);
    return res;
}

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(false); cin.tie(NULL); srand(time(NULL));

    string backup_filename = "GrandMaster_Auto_" + to_string(time(NULL)) + ".txt";
    if (argc > 1) backup_filename = string(argv[1]) + "_backup.txt"; 

    if (!(cin >> N)) return 0;
    
    original_grid.assign(N * N, 0); target_flat.assign((N - 2) * (N - 2), 0);
    global_visited.assign(N * N * N * N, 0); 

    for (int i = 0; i < N; ++i) for (int j = 0; j < N; ++j) cin >> original_grid[get_idx(i, j)];
    for (int i = 0; i < N - 2; ++i) for (int j = 0; j < N - 2; ++j) cin >> target_flat[i * (N - 2) + j];

    int M = N - 2;
    vector<pair<int, int>> master_order;
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
    for (const auto& l : layers) {
        int c_top = 2e9, c_left = 2e9;
        for (auto p : l) { c_top = min(c_top, p.first); c_left = min(c_left, p.second); }
        int idx_tl = 0;
        for (int i = 0; i < l.size(); ++i) { if (l[i].first == c_top && l[i].second == c_left) idx_tl = i; }
        int start_idx = (idx_tl + 1) % l.size();
        if(l.size() <= 1) { for(auto p: l) master_order.push_back(p); }
        else { for (int i = 0; i < l.size(); ++i) master_order.push_back(l[(start_idx + i) % l.size()]); }
    }

    vector<pair<int, int>> weights = {{1, 5}, {3, 5}, {1, 8}, {1, 10}, {1, 2}};

    cerr << "[INFO] Running GRAND MASTER Solver (IDA* Finisher + 1s Progress)...\n\n";

    int global_best_len = 2e9;
    string global_best_path = "";
    time_t start_time = time(NULL);
    time_t last_print_time = 0;
    int marathon_iters = 0;

    while (time(NULL) - start_time < 86400) {
        marathon_iters++;
        
        // 🧠 ปรับความฉลาดตามขนาดกระดาน
        int CHUNK_SIZE = (N > 20) ? (30 + rand() % 20) : (10 + rand() % 10); 
        int ITERS_PER_CHUNK = (N > 20) ? 20 : 100; // กระดานใหญ่ให้มั่วต่อรอบน้อยลง จะได้ไม่ค้าง

        vector<int> current_grid = original_grid;
        vector<bool> current_locked(N*N, false);
        int current_er = -1, current_ec = -1;
        for (int i = 0; i < N * N; ++i) {
            if (current_grid[i] == -1) { current_er = i / N; current_ec = i % N; break; }
        }
        
        string full_staged_path = "";
        bool staged_success = true;

        for (size_t chunk_start = 0; chunk_start < master_order.size(); chunk_start += CHUNK_SIZE) {
            size_t chunk_end = min(master_order.size(), chunk_start + CHUNK_SIZE);
            vector<pair<int, int>> chunk_targets(master_order.begin() + chunk_start, master_order.begin() + chunk_end);
            
            SimResult best_chunk; best_chunk.success = false;
            int min_chunk_len = 2e9;
            
            for (int iter = 0; iter < ITERS_PER_CHUNK; ++iter) {
                auto mutated_chunk = chunk_targets;
                if (mutated_chunk.size() > 2 && iter > 0) {
                    int mtype = rand() % 3;
                    if(mtype==0) { int idx=rand()%(mutated_chunk.size()-1); swap(mutated_chunk[idx], mutated_chunk[idx+1]); }
                    else if(mtype==1) { int idx1=rand()%mutated_chunk.size(); int idx2=rand()%mutated_chunk.size(); swap(mutated_chunk[idx1], mutated_chunk[idx2]); }
                    else { 
                        int idx1=rand()%mutated_chunk.size(); int idx2=rand()%mutated_chunk.size(); 
                        if(idx1>idx2) swap(idx1, idx2);
                        reverse(mutated_chunk.begin()+idx1, mutated_chunk.begin()+idx2);
                    }
                }
                
                int w_e = weights[rand() % weights.size()].first;
                int w_g = weights[rand() % weights.size()].second;
                int noise = (iter == 0) ? 0 : (rand() % 4); // รอบแรกของทุกปล้อง ขอเพอร์เฟกต์
                
                SimResult res = run_partial_simulation(mutated_chunk, w_e, w_g, noise, current_grid, current_locked, current_er, current_ec);
                if (res.success && res.path.length() < min_chunk_len) {
                    min_chunk_len = res.path.length(); best_chunk = res;
                }
            }
            
            if (!best_chunk.success) { staged_success = false; break; }
            
            current_grid = best_chunk.grid; current_locked = best_chunk.locked;
            current_er = best_chunk.er; current_ec = best_chunk.ec;
            full_staged_path += best_chunk.path;
            
            // ⏱️ อัปเดตหน้าจอ 1 วิ (ในลูปย่อยเลย!)
            time_t current_time = time(NULL);
            if (current_time - last_print_time >= 1) {
                last_print_time = current_time;
                int elap = current_time - start_time;
                cerr << "\r[MARATHON] Time: " << elap/3600 << "h " << (elap%3600)/60 << "m " << elap%60 << "s | Iters: " 
                     << marathon_iters << " | Global Best: " << (global_best_len == 2e9 ? 0 : global_best_len) << " moves      " << flush;
            }
        }

        if (!staged_success) continue; 
        full_staged_path = optimize_path(full_staged_path);
        
        if (full_staged_path.length() < global_best_len) {
            global_best_len = full_staged_path.length();
            global_best_path = full_staged_path;
            cerr << "\n\n🔥 THE FINISHER STRUCK! New Record: " << global_best_len << " \n";
            ofstream outfile(backup_filename);
            if (outfile.is_open()) {
                outfile << global_best_len << "\n" << global_best_path << "S\n"; outfile.close();
                cerr << "💾 Saved to '" << backup_filename << "'\n\n";
            }
        }
    }
    return 0;
}