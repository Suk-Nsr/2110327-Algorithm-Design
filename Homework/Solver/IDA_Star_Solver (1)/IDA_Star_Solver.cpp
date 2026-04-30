#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

int N;
vector<int> grid;
vector<int> target_flat;

inline int get_idx(int r, int c) { return r * N + c; }

// ฟังก์ชัน Heuristic (ระยะ Manhattan ของพื้นที่ตรงกลาง)
int get_heuristic() {
    int total_dist = 0;
    for (int tr = 0; tr < N - 2; ++tr) {
        for (int tc = 0; tc < N - 2; ++tc) {
            int expected = target_flat[tr * (N - 2) + tc];
            if (grid[get_idx(tr + 1, tc + 1)] == expected) continue;
            
            int min_dist = 1e9;
            for (int r = 0; r < N; ++r) {
                for (int c = 0; c < N; ++c) {
                    if (grid[get_idx(r, c)] == expected) {
                        int dist = abs((tr + 1) - r) + abs((tc + 1) - c);
                        if (dist < min_dist) min_dist = dist;
                    }
                }
            }
            total_dist += min_dist;
        }
    }
    return total_dist;
}

bool isGoal() {
    for (int i = 0; i < N - 2; ++i) {
        for (int j = 0; j < N - 2; ++j) {
            if (grid[get_idx(i + 1, j + 1)] != target_flat[i * (N - 2) + j]) return false;
        }
    }
    return true;
}

// ทิศทางการเดิน
int dr[] = {1, -1, 0, 0};
int dc[] = {0, 0, 1, -1};
char move_char[] = {'U', 'D', 'L', 'R'};

string best_path = "";
bool found_optimal = false;

// ฟังก์ชัน DFS แบบมีการจำกัดความลึกด้วย Heuristic (IDA*)
int search_ida(int g, int h, int er, int ec, string& path, char last_move, int threshold) {
    int f = g + h;
    
    // ถ้า Cost เกิน Threshold ปัจจุบัน ให้หยุดค้นหาเส้นทางนี้
    if (f > threshold) return f;
    
    // ถ้าเจอเป้าหมาย!
    if (isGoal()) {
        found_optimal = true;
        best_path = path;
        return f;
    }

    int min_threshold = 1e9; // เก็บค่า Threshold ถัดไปที่ต่ำที่สุด

    for (int i = 0; i < 4; ++i) {
        // ตัดการเดินย้อนกลับ
        if ((last_move == 'U' && move_char[i] == 'D') ||
            (last_move == 'D' && move_char[i] == 'U') ||
            (last_move == 'L' && move_char[i] == 'R') ||
            (last_move == 'R' && move_char[i] == 'L')) continue;

        int ner = er + dr[i];
        int nec = ec + dc[i];

        if (ner >= 0 && ner < N && nec >= 0 && nec < N) {
            // 1. Move (สลับที่)
            swap(grid[get_idx(er, ec)], grid[get_idx(ner, nec)]);
            path.push_back(move_char[i]);
            
            // คำนวณ Heuristic ใหม่
            int next_h = get_heuristic();

            // 2. เรียกตัวเอง (Recursion)
            int temp_t = search_ida(g + 1, next_h, ner, nec, path, move_char[i], threshold);
            
            if (found_optimal) return temp_t;
            if (temp_t < min_threshold) min_threshold = temp_t;

            // 3. Undo (สลับกลับคืน) สำคัญมาก! เพื่อไม่ให้ต้อง Copy Array
            path.pop_back();
            swap(grid[get_idx(er, ec)], grid[get_idx(ner, nec)]);
        }
    }
    return min_threshold;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    if (!(cin >> N)) return 0;

    grid.assign(N * N, 0);
    target_flat.assign((N - 2) * (N - 2), 0);
    
    int er = -1, ec = -1;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cin >> grid[get_idx(i, j)];
            if (grid[get_idx(i, j)] == -1) {
                er = i;
                ec = j;
            }
        }
    }

    for (int i = 0; i < N - 2; ++i) {
        for (int j = 0; j < N - 2; ++j) {
            cin >> target_flat[i * (N - 2) + j];
        }
    }

    int initial_h = get_heuristic();
    int threshold = initial_h; // เริ่มต้นขีดจำกัดที่ค่า Heuristic ต่ำสุดที่เป็นไปได้
    string current_path = "";

    cerr << "[INFO] Starting IDA* Search...\n";
    cerr << "[INFO] Minimum possible moves (Initial Heuristic): " << threshold << "\n";

    // วงลูปเพิ่มขอบเขตการค้นหาไปทีละนิด
    while (!found_optimal) {
        cerr << "[IDA*] Searching with max threshold: " << threshold << "...\r";
        
        int temp = search_ida(0, initial_h, er, ec, current_path, ' ', threshold);
        
        if (found_optimal) break;
        
        // ถ้าหาใน Threshold นี้ไม่เจอ ให้ขยับ Threshold ขึ้นไปเท่ากับค่าที่ต่ำที่สุดที่เพิ่งเจอ
        threshold = temp; 
    }

    cerr << "\n[SUCCESS] Optimal Solution Found!\n";
    cout << "\n-----------------------------------\n";
    cout << "Moves Count: " << best_path.length() << " (excluding 'S')\n";
    cout << best_path << "S\n";

    return 0;
}