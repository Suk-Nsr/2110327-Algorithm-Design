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

SimResult run_partial_simulation(
    const vector<pair<int, int>>& target_order, int w_empty, int w_goal, int noise_level,
    vector<int> grid, vector<bool> locked, int er, int ec) 
{
    SimResult res;
    res.path = "";
    
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
                    res.path += move_char[d];
                }
            }
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
                    
                    int crowded_penalty = 0;
                    int ddr[] = {1, -1, 0, 0};
                    int ddc[] = {0, 0, 1, -1};
                    for(int d=0; d<4; ++d) {
                        int nr = r + ddr[d], nc = c + ddc[d];
                        if(nr >= 0 && nr < N && nc >= 0 && nc < N) {
                            if(locked[get_idx(nr, nc)]) crowded_penalty += 20; 
                        } else {
                            crowded_penalty += 20; 
                        }
                    }

                    int random_noise = (noise_level > 0) ? (rand() % (noise_level * 2)) : 0;
                    int cost = (w_empty * dist_from_empty[get_idx(r, c)]) + 
                               (w_goal * dist_from_goal[get_idx(r, c)]) + 
                               crowded_penalty + random_noise + (is_perfect_match ? 10000 : 0);
                    
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
    
    res.grid = grid;
    res.locked = locked;
    res.er = er;
    res.ec = ec;
    res.success = true;
    res.path = optimize_path(res.path);
    return res;
}

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(false); cin.tie(NULL);
    srand(time(NULL));

    string backup_filename = "Staged_Beam_Auto_" + to_string(time(NULL)) + ".txt";
    if (argc > 1) backup_filename = string(argv[1]) + "_backup.txt"; 

    if (!(cin >> N)) return 0;
    
    original_grid.assign(N * N, 0); 
    target_flat.assign((N - 2) * (N - 2), 0);
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

    cerr << "[INFO] Running Staged Beam Chaos Engine...\n\n";

    int global_best_len = 2e9;
    string global_best_path = "";
    time_t start_time = time(NULL);
    time_t last_print_time = 0; // ตัวแปรสำหรับคุมเวลา 1 วินาที
    int marathon_iters = 0;

    while (time(NULL) - start_time < 86400) {
        marathon_iters++;
        
        int CHUNK_SIZE = 30 + (rand() % 30); 
        int ITERS_PER_CHUNK = 80; 

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
            
            SimResult best_chunk;
            best_chunk.success = false;
            int min_chunk_len = 2e9;
            
            for (int iter = 0; iter < ITERS_PER_CHUNK; ++iter) {
                auto mutated_chunk = chunk_targets;
                if (mutated_chunk.size() > 2) {
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
                int noise = rand() % 4;
                
                SimResult res = run_partial_simulation(mutated_chunk, w_e, w_g, noise, current_grid, current_locked, current_er, current_ec);
                if (res.success && res.path.length() < min_chunk_len) {
                    min_chunk_len = res.path.length();
                    best_chunk = res;
                }
            }
            
            if (!best_chunk.success) {
                best_chunk = run_partial_simulation(chunk_targets, 1, 5, 0, current_grid, current_locked, current_er, current_ec);
                if (!best_chunk.success) { staged_success = false; break; }
            }
            
            current_grid = best_chunk.grid;
            current_locked = best_chunk.locked;
            current_er = best_chunk.er;
            current_ec = best_chunk.ec;
            full_staged_path += best_chunk.path;
        }

        if (!staged_success) continue; 
        full_staged_path = optimize_path(full_staged_path);
        
        // ⏱️ อัปเดต Progress Bar ทุกๆ 1 วินาทีเป๊ะๆ
        time_t current_time = time(NULL);
        if (current_time - last_print_time >= 1) {
            last_print_time = current_time;
            int elapsed_seconds = current_time - start_time;
            int h = elapsed_seconds / 3600;
            int m = (elapsed_seconds % 3600) / 60;
            int s = elapsed_seconds % 60;
            cerr << "\r[MARATHON] Time: " << h << "h " << m << "m " << s << "s | Iters: " << marathon_iters 
                 << " | Global Best: " << (global_best_len == 2e9 ? 0 : global_best_len) << " moves      " << flush;
        }

        if (full_staged_path.length() < global_best_len) {
            global_best_len = full_staged_path.length();
            global_best_path = full_staged_path;
            
            cerr << "\n\n🔥 STAGED BEAM RECORD BROKEN! Moves: " << global_best_len << " \n";
            
            ofstream outfile(backup_filename);
            if (outfile.is_open()) {
                outfile << global_best_len << "\n" << global_best_path << "S\n";
                outfile.close();
                cerr << "💾 Saved to '" << backup_filename << "'\n\n";
            }
        }
    }

    cerr << "\n\n[SUCCESS] Completed Marathon.\n";
    cout << "\n-----------------------------------\n";
    cout << "Best Moves: " << global_best_len << "\n";
    cout << global_best_path << "S\n";

    return 0;
}