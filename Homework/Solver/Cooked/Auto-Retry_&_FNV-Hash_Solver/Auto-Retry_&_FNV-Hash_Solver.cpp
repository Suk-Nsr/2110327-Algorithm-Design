#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <cstdint>

using namespace std;

int N;
vector<int> target_flat;

inline int get_idx(int r, int c) { return r * N + c; }

// สุดยอดสมการ Hashing (FNV-1a) ช่วยบีบอัด Memory ไม่ให้คอมค้าง
uint64_t compute_hash(const vector<int8_t>& v) {
    uint64_t hash = 14695981039346656037ULL;
    for (int8_t x : v) {
        hash ^= x;
        hash *= 1099511628211ULL;
    }
    return hash;
}

struct State {
    vector<int8_t> grid;
    int gap_r, gap_c;
    string path;
    int f_score;
};

// ฟังก์ชันคำนวณคะแนน ยอมให้ของเก่าพังได้ แต่บวก Penalty มหาศาลเพื่อบังคับให้ซ่อม
int calculateStagedHeuristic(const vector<int8_t>& grid, const vector<pair<int, int>>& active_goals, int er, int ec) {
    int h = 0;
    
    // 1. ตรวจสอบเป้าหมายเก่าที่ทำเสร็จไปแล้ว
    for (size_t i = 0; i < active_goals.size() - 1; ++i) {
        int gr = active_goals[i].first;
        int gc = active_goals[i].second;
        int expected = target_flat[(gr - 1) * (N - 2) + (gc - 1)];
        if (grid[get_idx(gr, gc)] != expected) {
            h += 10000; // Penalty หนักมาก! (ห้ามทิ้ง ต้องซ่อมก่อน)
        }
    }
    
    // 2. โฟกัสเป้าหมายใหม่ล่าสุด
    int new_gr = active_goals.back().first;
    int new_gc = active_goals.back().second;
    int new_expected = target_flat[(new_gr - 1) * (N - 2) + (new_gc - 1)];
    
    if (grid[get_idx(new_gr, new_gc)] != new_expected) {
        int min_cost = 1e9;
        for (int r = 0; r < N; ++r) {
            for (int c = 0; c < N; ++c) {
                if (grid[get_idx(r, c)] == new_expected) {
                    // ห้ามขโมยแผ่นของเก่าที่เสร็จแล้วเด็ดขาด
                    bool stealing = false;
                    for (size_t i = 0; i < active_goals.size() - 1; ++i) {
                        if (active_goals[i].first == r && active_goals[i].second == c) {
                            stealing = true; break;
                        }
                    }
                    if (stealing) continue;
                    
                    int dist_to_target = abs(new_gr - r) + abs(new_gc - c);
                    int dist_to_empty = abs(r - er) + abs(c - ec);
                    
                    int cost = dist_to_empty + (5 * dist_to_target);
                    if (cost < min_cost) min_cost = cost;
                }
            }
        }
        h += min_cost;
    }
    return h;
}

State solve_beam_phase(State start_state, const vector<pair<int, int>>& active_goals) {
    int initial_h = calculateStagedHeuristic(start_state.grid, active_goals, start_state.gap_r, start_state.gap_c);
    if (initial_h == 0) return start_state;

    // ระบบสู้ไม่ถอย: ถ้าหาไม่เจอ จะเพิ่ม Beam Width เรื่อยๆ เพื่อเจาะกำแพง
    vector<int> beam_widths = {500, 2000, 5000, 15000}; 
    
    // ทิศทางที่ถูกต้อง (อิงตามการเลื่อนกระเบื้อง Tile)
    int dr[] = {1, -1, 0, 0};  // U: ช่องว่างลง, D: ช่องว่างขึ้น
    int dc[] = {0, 0, 1, -1};  // L: ช่องว่างขวา, R: ช่องว่างซ้าย
    char move_char[] = {'U', 'D', 'L', 'R'};

    for (int bw : beam_widths) {
        unordered_set<uint64_t> visited;
        vector<State> beam;
        
        start_state.f_score = initial_h;
        start_state.path = ""; 
        beam.push_back(start_state);
        visited.insert(compute_hash(start_state.grid));

        int max_depth = 400; // โควตาในการเดินสูงสุดต่อ 1 แผ่น

        for (int depth = 1; depth <= max_depth; ++depth) {
            vector<State> next_states;
            next_states.reserve(beam.size() * 3);
            
            for (const auto& current : beam) {
                char last_move = current.path.empty() ? ' ' : current.path.back();

                for (int i = 0; i < 4; ++i) {
                    // ดักการเดินย้อนกลับ
                    if ((last_move == 'U' && move_char[i] == 'D') ||
                        (last_move == 'D' && move_char[i] == 'U') ||
                        (last_move == 'L' && move_char[i] == 'R') ||
                        (last_move == 'R' && move_char[i] == 'L')) continue;

                    int ner = current.gap_r + dr[i];
                    int nec = current.gap_c + dc[i];

                    // เช็คว่าช่องว่างไม่ออกนอกกระดาน
                    if (ner >= 0 && ner < N && nec >= 0 && nec < N) {
                        State next_state = current;
                        swap(next_state.grid[get_idx(current.gap_r, current.gap_c)], 
                             next_state.grid[get_idx(ner, nec)]);
                        
                        uint64_t hash_val = compute_hash(next_state.grid);
                        
                        if (visited.find(hash_val) == visited.end()) {
                            next_state.gap_r = ner;
                            next_state.gap_c = nec;
                            next_state.path += move_char[i];
                            
                            int h = calculateStagedHeuristic(next_state.grid, active_goals, ner, nec);
                            if (h == 0) return next_state; // แผ่นนี้เข้าที่แล้ว! สำเร็จ!

                            next_state.f_score = next_state.path.length() + h; 
                            next_states.push_back(next_state);
                            visited.insert(hash_val);
                        }
                    }
                }
            }

            if (next_states.empty()) break; // ตัน

            sort(next_states.begin(), next_states.end(), [](const State& a, const State& b) {
                return a.f_score < b.f_score;
            });

            beam.clear();
            for (int i = 0; i < min((int)next_states.size(), bw); ++i) {
                beam.push_back(next_states[i]);
            }
        }
        cerr << "\n[WARN] Phase failed. Retrying with Wider Beam (Width = " << bw << ")...\n";
    }
    
    cerr << "\n[FATAL ERROR] Impossible Board state or heavily stuck!\n";
    exit(1); 
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

    State current_state;
    current_state.grid.assign(N * N, 0);
    
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int val;
            cin >> val;
            current_state.grid[get_idx(i, j)] = (int8_t)val;
            if (val == -1) {
                current_state.gap_r = i;
                current_state.gap_c = j;
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
    vector<pair<int, int>> active_goals;
    int count_done = 0;
    int total_targets = (N - 2) * (N - 2);

    cerr << "[INFO] Starting Robust Hybrid Solver...\n";

    // เดินแบบงูเลื้อย (Snake Pattern) ป้องกันการเกิด Cul-de-sac (ทางตัน)
    for (int i = 1; i <= N - 2; ++i) {
        for (int step = 1; step <= N - 2; ++step) {
            int j = (i % 2 != 0) ? step : (N - 2 - step + 1); 
            
            active_goals.push_back({i, j}); 

            // ส่งกระดานเข้าไปหาทางเดิน
            current_state = solve_beam_phase(current_state, active_goals);
            
            // สะสมเส้นทาง
            final_answer += current_state.path;
            
            count_done++;
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