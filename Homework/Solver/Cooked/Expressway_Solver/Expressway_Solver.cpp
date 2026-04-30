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

// 🧠 A* Algorithm (พร้อมตาทิพย์ Hovering Penalty)
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
                    
                    // Hovering Penalty ป้องกันช่องว่างเดินขวางทาง
                    if (goal_c > ntc && nec < ntc) penalty += 6; 
                    if (goal_c < ntc && nec > ntc) penalty += 6; 
                    if (goal_r > ntr && ner < ntr) penalty += 6; 
                    if (goal_r < ntr && ner > ntr) penalty += 6; 

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

// =========================================================
// 🎯 ระบบ Global Pre-Assignment & Deterministic Tracking
// =========================================================
vector<int> tile_pos; // เก็บพิกัดปัจจุบันของกระเบื้อง ID ต่างๆ
vector<int> grid_id;  // เก็บว่าพิกัดนี้ มีกระเบื้อง ID อะไรอยู่

// ฟังก์ชันสร้างบัตรประชาชนให้กระเบื้องและจับคู่ผูกขาด (มีระบบ Chaos Noise แทรกอยู่)
vector<int> generate_global_assignment(const vector<pair<int, int>>& target_order, int noise_level) {
    vector<int> assigned_target_id(N * N, -1);
    vector<bool> used(N * N, false);
    
    for (auto pos : target_order) {
        int tr = pos.first, tc = pos.second;
        int goal_val = target_flat[(tr - 1) * (N - 2) + (tc - 1)];
        
        int best_id = -1;
        int min_dist = 2e9;
        
        // ค้นหากระเบื้องที่ยังว่างและสีตรงกัน
        for (int i = 0; i < N * N; ++i) {
            if (!used[i] && original_grid[i] == goal_val) {
                int r = i / N, c = i % N;
                // บวก Noise เข้าไปในระยะทางเพื่อให้เกิดการจับคู่ใหม่ๆ ในแต่ละรอบ
                int dist = abs(tr - r) + abs(tc - c) + (noise_level > 0 ? (rand() % noise_level) : 0);
                if (dist < min_dist) {
                    min_dist = dist;
                    best_id = i;
                }
            }
        }
        assigned_target_id[get_idx(tr, tc)] = best_id;
        used[best_id] = true;
    }
    return assigned_target_id;
}

// จำลองการแก้กระดานแบบ Fast Track (ไม่ต้องสแกนหา min_cost อีกต่อไป)
string run_expressway_simulation(const vector<pair<int, int>>& target_order, const vector<int>& assigned_target_id, int noise_level) {
    tile_pos.resize(N * N);
    grid_id.resize(N * N);
    vector<bool> locked(N * N, false);
    int er = -1, ec = -1;

    // เริ่มต้นระบบ ID Tracking
    for (int i = 0; i < N * N; ++i) {
        tile_pos[i] = i;
        grid_id[i] = i;
        if (original_grid[i] == -1) { er = i / N; ec = i % N; }
    }

    string final_answer = "";

    for (auto pos : target_order) {
        int r = pos.first;
        int c = pos.second;
        int target_id = assigned_target_id[get_idx(r, c)];
        
        // ชี้เป้าตำแหน่งกระเบื้องแบบ O(1)
        int current_pos = tile_pos[target_id];
        int best_tr = current_pos / N;
        int best_tc = current_pos % N;

        if (best_tr == r && best_tc == c) {
            locked[get_idx(r, c)] = true;
            continue;
        }

        // โหมด Earthquake เขย่าหลบมุม
        if (noise_level > 0 && rand() % 100 < 5) {
            int steps = rand() % 3 + 1;
            int dr[] = {1, -1, 0, 0}, dc[] = {0, 0, 1, -1};
            char mc[] = {'U', 'D', 'L', 'R'};
            for (int k = 0; k < steps; ++k) {
                int d = rand() % 4;
                int ner = er + dr[d], nec = ec + dc[d];
                if (ner >= 0 && ner < N && nec >= 0 && nec < N && !locked[get_idx(ner, nec)]) {
                    // อัปเดตระบบ ID ทุกครั้งที่ขยับ
                    int id1 = grid_id[get_idx(er, ec)];
                    int id2 = grid_id[get_idx(ner, nec)];
                    swap(grid_id[get_idx(er, ec)], grid_id[get_idx(ner, nec)]);
                    tile_pos[id1] = get_idx(ner, nec);
                    tile_pos[id2] = get_idx(er, ec);
                    er = ner; ec = nec;
                    final_answer += mc[d];
                }
            }
            // อัปเดตตำแหน่งกระเบื้องอีกครั้งหลังจากเขย่า
            current_pos = tile_pos[target_id];
            best_tr = current_pos / N;
            best_tc = current_pos % N;
        }

        string path = solve_single_tile(best_tr, best_tc, er, ec, r, c, locked);
        if (path == "FAILED") return "FAILED";

        for (char m : path) {
            int ner = er, nec = ec;
            if (m == 'U') ner++; else if (m == 'D') ner--; else if (m == 'L') nec++; else if (m == 'R') nec--;
            
            // 🛡️ หัวใจของความเร็ว: อัปเดตระบบ Tracking สดๆ ร้อนๆ
            int id1 = grid_id[get_idx(er, ec)];
            int id2 = grid_id[get_idx(ner, nec)];
            swap(grid_id[get_idx(er, ec)], grid_id[get_idx(ner, nec)]);
            tile_pos[id1] = get_idx(ner, nec);
            tile_pos[id2] = get_idx(er, ec);
            
            er = ner; ec = nec;
        }
        final_answer += path;
        locked[get_idx(r, c)] = true;
    }
    return optimize_path(final_answer);
}

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(false); cin.tie(NULL);
    srand(time(NULL));

    string backup_filename;
    if (argc > 1) backup_filename = string(argv[1]) + "_backup.txt"; 
    else backup_filename = "Expressway_Auto_" + to_string(time(NULL)) + ".txt";

    if (!(cin >> N)) return 0;
    
    original_grid.assign(N * N, 0); 
    target_flat.assign((N - 2) * (N - 2), 0);
    global_visited.assign(N * N * N * N, 0); 

    for (int i = 0; i < N; ++i) for (int j = 0; j < N; ++j) cin >> original_grid[get_idx(i, j)];
    for (int i = 0; i < N - 2; ++i) for (int j = 0; j < N - 2; ++j) cin >> target_flat[i * (N - 2) + j];

    // 🛣️ สร้างเส้นทาง Typewriter Order (ซ้าย->ขวา, บน->ลงล่าง) เปิดลานกว้างด้านล่างไว้เป็นทางด่วน
    vector<pair<int, int>> typewriter_order;
    for (int r = 1; r <= N - 2; ++r) {
        for (int c = 1; c <= N - 2; ++c) {
            typewriter_order.push_back({r, c});
        }
    }
    
    // 🛣️ ทางเลือกสำรอง: Vertical Typewriter (บน->ล่าง, ซ้าย->ขวา) เปิดลานกว้างด้านขวา
    vector<pair<int, int>> vertical_typewriter_order;
    for (int c = 1; c <= N - 2; ++c) {
        for (int r = 1; r <= N - 2; ++r) {
            vertical_typewriter_order.push_back({r, c});
        }
    }

    vector<pair<string, vector<pair<int, int>>>> strategies = {
        {"Standard Typewriter", typewriter_order},
        {"Vertical Typewriter", vertical_typewriter_order}
    };

    cerr << "[INFO] Running EXPRESSWAY Engine (No-Steal Global Assignment)...\n\n";

    string best_path = "";
    int min_len = 2e9;
    string best_name = "";
    
    time_t start_time = time(NULL);
    time_t last_print = time(NULL);
    int iterations = 0;
    
    while (time(NULL) - start_time < 86400) {
        iterations++;
        
        try {
            int strat_idx = rand() % strategies.size();
            auto strategy = strategies[strat_idx];
            vector<pair<int, int>> order = strategy.second;
            
            // รอบแรกของทุกๆ 10 รอบ ให้ลองแบบ Perfect Greedy (ไม่มี Noise)
            int noise = (iterations % 10 == 0) ? 0 : (rand() % 15 + 1); 
            
            // 🔥 สร้างแผนการจับคู่ล่วงหน้า
            vector<int> assigned_ids = generate_global_assignment(order, noise);
            
            // 🚀 วิ่งตะลุยด้วยความเร็วแสง
            string path = run_expressway_simulation(order, assigned_ids, noise/2);
            
            time_t current_time = time(NULL);
            if (current_time - last_print >= 1) {
                last_print = current_time;
                int elapsed = current_time - start_time;
                cerr << "\r[PROGRESS] Time: " << elapsed/3600 << "h " << (elapsed%3600)/60 << "m " << elapsed%60 << "s "
                     << "| Iters: " << iterations 
                     << "| Best: " << (min_len == 2e9 ? 0 : min_len) << " moves      " << flush;
            }
            
            if (path != "FAILED" && path.length() < min_len) {
                best_path = path;
                min_len = path.length();
                best_name = strategy.first + " [AssignNoise: " + to_string(noise) + "]";
                
                cerr << "\n\n🔥 EXPRESSWAY RECORD! Moves: " << min_len << " (" << best_name << ")\n";
                
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

    cerr << "\n\n[SUCCESS] Completed Marathon.\n";
    cout << "\n-----------------------------------\n";
    cout << "Best Moves: " << best_path.length() << "\n" << best_path << "S\n";
    return 0;
}