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

// FNV-1a Hash สำหรับบีบอัดสถานะกระดาน
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

// Heuristic ระบบ Soft Lock: ยอมให้พังได้ แต่บวก Penalty มหาศาลเพื่อบังคับให้ซ่อม
int calculateStagedHeuristic(const vector<int8_t>& grid, const vector<pair<int, int>>& active_goals, int er, int ec) {
    int h = 0;
    
    // 1. ตรวจสอบเป้าหมายเก่า (วงแหวนชั้นใน) ที่ทำเสร็จไปแล้ว
    for (size_t i = 0; i < active_goals.size() - 1; ++i) {
        int gr = active_goals[i].first;
        int gc = active_goals[i].second;
        int expected = target_flat[(gr - 1) * (N - 2) + (gc - 1)];
        if (grid[get_idx(gr, gc)] != expected) {
            h += 10000; // Penalty! ยอมให้พังได้เป็นทางผ่าน แต่ตอนจบเฟสนี้ต้องซ่อมกลับมา
        }
    }
    
    // 2. โฟกัสเป้าหมายใหม่ล่าสุด (ช่องที่กำลังทำ)
    int new_gr = active_goals.back().first;
    int new_gc = active_goals.back().second;
    int new_expected = target_flat[(new_gr - 1) * (N - 2) + (new_gc - 1)];
    
    if (grid[get_idx(new_gr, new_gc)] != new_expected) {
        int min_cost = 1e9;
        for (int r = 0; r < N; ++r) {
            for (int c = 0; c < N; ++c) {
                if (grid[get_idx(r, c)] == new_expected) {
                    // ห้ามขโมยแผ่นจากวงแหวนที่ทำเสร็จแล้ว
                    bool stealing = false;
                    for (size_t i = 0; i < active_goals.size() - 1; ++i) {
                        if (active_goals[i].first == r && active_goals[i].second == c) {
                            stealing = true; break;
                        }
                    }
                    if (stealing) continue;
                    
                    // เนื่องจากเป็น Soft Lock (ไม่มีกำแพงจริง) จึงใช้ Manhattan Distance ได้เลย
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
    if (initial_h == 0) return start_state; // ถ้าเข้าที่อยู่แล้ว ข้ามเลย!

    // ระบบเพิ่มความกว้าง Beam อัตโนมัติ (สู้ไม่ถอย)
    // สำหรับ Spiral ตอนจบวงแหวนมักจะเจองานยาก (Cul-de-sac) Beam จะขยายตัวเองไปเจาะทะลวงให้
    vector<int> beam_widths = {500, 2000, 5000, 15000, 30000}; 
    
    // ทิศทางอิงตาม Visualizer (Tile Move)
    int dr[] = {1, -1, 0, 0};
    int dc[] = {0, 0, 1, -1};
    char move_char[] = {'U', 'D', 'L', 'R'};

    for (int bw : beam_widths) {
        unordered_set<uint64_t> visited;
        vector<State> beam;
        
        start_state.f_score = initial_h;
        start_state.path = ""; 
        beam.push_back(start_state);
        visited.insert(compute_hash(start_state.grid));

        int max_depth = 400; // วิ่งได้ลึกสุดต่อการทำ 1 ช่อง

        for (int depth = 1; depth <= max_depth; ++depth) {
            vector<State> next_states;
            next_states.reserve(beam.size() * 3);
            
            for (const auto& current : beam) {
                char last_move = current.path.empty() ? ' ' : current.path.back();

                for (int i = 0; i < 4; ++i) {
                    // ตัดการเดินย้อนกลับ
                    if ((last_move == 'U' && move_char[i] == 'D') ||
                        (last_move == 'D' && move_char[i] == 'U') ||
                        (last_move == 'L' && move_char[i] == 'R') ||
                        (last_move == 'R' && move_char[i] == 'L')) continue;

                    int ner = current.gap_r + dr[i];
                    int nec = current.gap_c + dc[i];

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
                            
                            if (h == 0) return next_state; // ซ่อมของเก่าเสร็จ + ของใหม่เข้าที่ = ชนะเฟสนี้!

                            next_state.f_score = next_state.path.length() + h; 
                            next_states.push_back(next_state);
                            visited.insert(hash_val);
                        }
                    }
                }
            }

            if (next_states.empty()) break; 

            sort(next_states.begin(), next_states.end(), [](const State& a, const State& b) {
                return a.f_score < b.f_score;
            });

            beam.clear();
            for (int i = 0; i < min((int)next_states.size(), bw); ++i) {
                beam.push_back(next_states[i]);
            }
        }
        cerr << "\n[WARN] Phase stuck (Cul-de-sac detected). Retrying with Wider Beam (Width = " << bw << ")...\n";
    }
    
    cerr << "\n[FATAL ERROR] Impossible Board state or heavily stuck!\n";
    exit(1); 
}

// ฟังก์ชันตัดการย้อนกลับที่อาจหลุดมาจากการต่อเฟส
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

    // 1. สร้างลำดับเป้าหมายแบบ Inside-Out Spiral (ก้นหอยจากตรงกลาง)
    vector<pair<int, int>> target_order;
    int center = N / 2;
    for (int step = 0; step <= (N - 2) / 2; ++step) {
        vector<pair<int, int>> ring;
        for (int i = 1; i <= N - 2; ++i) {
            for (int j = 1; j <= N - 2; ++j) {
                if (max(abs(i - center), abs(j - center)) == step) {
                    ring.push_back({i, j});
                }
            }
        }
        // เรียงให้เป็นวงกลมต่อเนื่องกัน
        sort(ring.begin(), ring.end(), [center](const pair<int,int>& a, const pair<int,int>& b) {
            return atan2(a.first - center, a.second - center) < atan2(b.first - center, b.second - center);
        });
        for (auto p : ring) target_order.push_back(p);
    }

    string final_answer = "";
    vector<pair<int, int>> active_goals;
    int count_done = 0;
    int total_targets = (N - 2) * (N - 2);

    cerr << "[INFO] Starting Ultimate Spiral Soft-Lock Solver...\n";

    // 2. ลุยแก้ทีละเป้าหมายตามลำดับก้นหอย
    for (auto goal_pos : target_order) {
        active_goals.push_back(goal_pos); 

        // ส่งกระดานเข้าไปหาทางเดิน (ถ้าเจอทางตันมันจะเพิ่ม Beam เอง)
        current_state = solve_beam_phase(current_state, active_goals);
        
        final_answer += current_state.path;
        
        count_done++;
        cerr << "[PROGRESS] Solved " << count_done << "/" << total_targets << " tiles...    \r";
    }

    final_answer = optimize_path(final_answer);

    cerr << "\n\n[SUCCESS] Board solved completely!\n";
    cout << "\n-----------------------------------\n";
    cout << "Total Moves: " << final_answer.length() << " (excluding 'S')\n";
    cout << final_answer << "S\n";

    return 0;
}