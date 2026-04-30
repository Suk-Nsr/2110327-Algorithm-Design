#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

using namespace std;

struct State {
    vector<int> grid;
    int gap_r, gap_c;
    int mismatches;
    string path;
    int f_score; 
};

int N;
vector<int> target_flat;

inline int get_idx(int r, int c) { return r * N + c; }
inline bool in_target_area(int r, int c) { return r >= 1 && r <= N - 2 && c >= 1 && c <= N - 2; }

int countInitialMismatches(const vector<int>& grid) {
    int m = 0;
    for (int i = 0; i < N - 2; ++i) {
        for (int j = 0; j < N - 2; ++j) {
            if (grid[get_idx(i + 1, j + 1)] != target_flat[i * (N - 2) + j]) m++;
        }
    }
    return m;
}

int calculateHeuristic(const vector<int>& grid, int mismatches) {
    int total_dist = 0;
    for (int tr = 0; tr < N - 2; ++tr) {
        for (int tc = 0; tc < N - 2; ++tc) {
            int expected_val = target_flat[tr * (N - 2) + tc];
            if (grid[get_idx(tr + 1, tc + 1)] == expected_val) continue;
            
            int min_dist = 1e9;
            for (int r = 0; r < N; ++r) {
                for (int c = 0; c < N; ++c) {
                    if (grid[get_idx(r, c)] == expected_val) {
                        int dist = abs((tr + 1) - r) + abs((tc + 1) - c);
                        if (dist < min_dist) min_dist = dist;
                    }
                }
            }
            total_dist += min_dist;
        }
    }
    return total_dist + (mismatches * 3);
}

bool isReverseMove(char last_move, char current_move) {
    if (last_move == 'U' && current_move == 'D') return true;
    if (last_move == 'D' && current_move == 'U') return true;
    if (last_move == 'L' && current_move == 'R') return true;
    if (last_move == 'R' && current_move == 'L') return true;
    return false;
}

string solveBeamSearch(State start_state, int beam_width) {
    if (start_state.mismatches == 0) return "S";

    vector<State> beam;
    start_state.f_score = calculateHeuristic(start_state.grid, start_state.mismatches);
    beam.push_back(start_state);

    int dr[] = {1, -1, 0, 0};
    int dc[] = {0, 0, 1, -1};
    char move_char[] = {'U', 'D', 'L', 'R'};

    // ปรับ Max Depth ให้ลึกขึ้นมาก เผื่อ Test Case ขนาดใหญ่ (เช่น N=67)
    int max_depth = 20000; 

    for (int depth = 1; depth <= max_depth; ++depth) {
        vector<State> next_states;
        next_states.reserve(beam.size() * 3); 

        int best_mismatches_current = 1e9;

        for (const auto& current : beam) {
            char last_move = current.path.empty() ? ' ' : current.path.back();

            for (int i = 0; i < 4; ++i) {
                if (isReverseMove(last_move, move_char[i])) continue;

                int nr = current.gap_r + dr[i];
                int nc = current.gap_c + dc[i];

                if (nr >= 0 && nr < N && nc >= 0 && nc < N) {
                    State next_state = current;
                    
                    int gap_idx = get_idx(current.gap_r, current.gap_c);
                    int tile_idx = get_idx(nr, nc);
                    int tile_val = next_state.grid[tile_idx];

                    if (in_target_area(nr, nc)) {
                        if (tile_val == target_flat[(nr - 1) * (N - 2) + (nc - 1)]) {
                            next_state.mismatches++;
                        }
                    }
                    if (in_target_area(current.gap_r, current.gap_c)) {
                        if (tile_val == target_flat[(current.gap_r - 1) * (N - 2) + (current.gap_c - 1)]) {
                            next_state.mismatches--;
                        }
                    }

                    swap(next_state.grid[gap_idx], next_state.grid[tile_idx]);
                    next_state.gap_r = nr;
                    next_state.gap_c = nc;
                    next_state.path += move_char[i];
                    
                    if (next_state.mismatches == 0) {
                        cerr << "\n[SUCCESS] Found solution at depth " << depth << "!\n";
                        return next_state.path + "S";
                    }

                    int h_score = calculateHeuristic(next_state.grid, next_state.mismatches);
                    next_state.f_score = next_state.path.length() + h_score; 
                    
                    next_states.push_back(next_state);
                    best_mismatches_current = min(best_mismatches_current, next_state.mismatches);
                }
            }
        }

        if (depth % 10 == 0) {
            cerr << "[PROGRESS] Depth: " << depth 
                 << " | Beam Size: " << beam.size() 
                 << " | Best Mismatches Left: " << best_mismatches_current << "    \r";
        }

        sort(next_states.begin(), next_states.end(), [](const State& a, const State& b) {
            return a.f_score < b.f_score;
        });

        beam.clear();
        for (int i = 0; i < min((int)next_states.size(), beam_width); ++i) {
            beam.push_back(next_states[i]);
        }
        
        if (beam.empty()) break;
    }
    cerr << "\n";
    return "No solution found within max_depth";
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    if (!(cin >> N)) return 0;

    State start_state;
    start_state.grid.assign(N * N, 0);
    
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int val;
            cin >> val;
            start_state.grid[get_idx(i, j)] = val;
            if (val == -1) {
                start_state.gap_r = i;
                start_state.gap_c = j;
            }
        }
    }

    target_flat.assign((N - 2) * (N - 2), 0);
    for (int i = 0; i < N - 2; ++i) {
        for (int j = 0; j < N - 2; ++j) {
            cin >> target_flat[i * (N - 2) + j];
        }
    }

    start_state.mismatches = countInitialMismatches(start_state.grid);

    cerr << "[INFO] Starting search...\n";
    cerr << "[INFO] Initial Mismatches: " << start_state.mismatches << "\n";

    // --- ขยาย Beam Width ให้กว้างมาก ---
    // ยิ่งกว้างยิ่งได้คำตอบที่สั้น แต่จะกิน RAM และเวลารันนานขึ้น
    // สามารถปรับขึ้นเป็น 20000 หรือ 50000 ได้ถ้าคอมพิวเตอร์ไหว
    int BEAM_WIDTH = 15000; 
    string answer = solveBeamSearch(start_state, BEAM_WIDTH);

    cout << "\n-----------------------------------\n";
    if (answer.back() == 'S') {
        cout << "Moves Count: " << answer.length() - 1 << " (excluding 'S')\n";
    }
    cout << answer << "\n";

    return 0;
}