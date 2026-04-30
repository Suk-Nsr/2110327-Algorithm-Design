#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <ctime>
#include <cstdint>

using namespace std;

int N;

// ค้นหาแบบพุ่งเป้า (IDA*) โดยจำกัดความลึกสูงสุดที่ max_depth
bool search_ida(vector<int16_t>& grid, int er, int ec, int g, int h, int bound, int& next_bound, string& path, int last_move, const vector<int>& target_r, const vector<int>& target_c, int max_depth) {
    int f = g + h;
    if (f > bound) {
        if (f < next_bound) next_bound = f;
        return false;
    }
    if (h == 0) return true; // ทะลุถึงปลายทางแล้ว!
    if (g >= max_depth) return false; // ป้องกันการหาลึกเกินไป

    int dr[] = {1, -1, 0, 0};
    int dc[] = {0, 0, 1, -1};
    char mc[] = {'U', 'D', 'L', 'R'};
    int opp[] = {1, 0, 3, 2}; // ป้องกันเดินย้อนกลับ

    for (int i = 0; i < 4; ++i) {
        if (last_move != -1 && i == opp[last_move]) continue;
        
        int ner = er + dr[i], nec = ec + dc[i];
        if (ner >= 0 && ner < N && nec >= 0 && nec < N) {
            int val = grid[ner * N + nec];
            int delta_h = 0;
            
            // คำนวณ Heuristic แบบ O(1)
            if (val > 0 && target_r[val] != -1) {
                delta_h = (abs(er - target_r[val]) + abs(ec - target_c[val])) - (abs(ner - target_r[val]) + abs(nec - target_c[val]));
            }
            
            swap(grid[er * N + ec], grid[ner * N + nec]);
            path += mc[i];
            
            if (search_ida(grid, ner, nec, g + 1, h + delta_h, bound, next_bound, path, i, target_r, target_c, max_depth)) {
                return true;
            }
            
            path.pop_back();
            swap(grid[er * N + ec], grid[ner * N + nec]);
        }
    }
    return false;
}

// 🎯 ฟังก์ชันหาทางลัดด้วย Sniper IDA*
string find_long_range_shortcut(vector<int16_t>& start_grid, int s_er, int s_ec, const vector<int16_t>& end_grid, int W) {
    int max_val = N * N;
    vector<int> target_r(max_val + 1, -1);
    vector<int> target_c(max_val + 1, -1);
    
    // สร้างพิกัดเป้าหมายจาก end_grid
    for (int i = 0; i < N * N; ++i) {
        int val = end_grid[i];
        if (val > 0) {
            target_r[val] = i / N;
            target_c[val] = i % N;
        }
    }

    // ประเมินระยะแมนฮัตตันขั้นต่ำ
    int initial_h = 0;
    for (int i = 0; i < N * N; ++i) {
        int val = start_grid[i];
        if (val > 0 && target_r[val] != -1) {
            initial_h += abs((i / N) - target_r[val]) + abs((i % N) - target_c[val]);
        }
    }

    // ถ้าแมนฮัตตันบอกว่าต้องใช้ก้าวเยอะกว่า W-1 แปลว่าไม่มีทางลัดแน่นอน! สกิปเลย!
    if (initial_h > W - 1) return "FAILED";

    string path = "";
    int bound = initial_h;
    int max_depth = W - 1; // บังคับว่าต้องสั้นกว่าของเดิม (W) อย่างน้อย 1 ก้าว

    while (bound <= max_depth) {
        int next_bound = 1e9;
        if (search_ida(start_grid, s_er, s_ec, 0, initial_h, bound, next_bound, path, -1, target_r, target_c, max_depth)) {
            return path;
        }
        if (next_bound == 1e9) break;
        bound = next_bound;
    }
    return "FAILED";
}

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(false); cin.tie(NULL);
    if (argc < 3) {
        cerr << "[ERROR] Usage: ./Sniper.exe <input.in> <solution.txt> [WindowSize]\n";
        return 1;
    }

    ifstream fin(argv[1]);
    if (!(fin >> N)) return 0;
    
    vector<int16_t> initial_grid(N * N);
    int init_er = -1, init_ec = -1;
    for (int i = 0; i < N * N; ++i) {
        int val; fin >> val; initial_grid[i] = (int16_t)val;
        if (val == -1) { init_er = i / N; init_ec = i % N; }
    }
    fin.close();

    ifstream fsol(argv[2]);
    string original_path = "";
    string temp;
    while (fsol >> temp) {
        string seg = "";
        for (char c : temp) if (c == 'U' || c == 'D' || c == 'L' || c == 'R') seg += c;
        if (seg.length() > original_path.length()) original_path = seg;
    }
    fsol.close();

    // กำหนดขนาดหน้าต่าง (แนะนำ: 24, 30, 40)
    int W = (argc > 3) ? atoi(argv[3]) : 24; 
    
    cerr << "[INFO] SNIPER COMPRESSOR INITIALIZED\n";
    cerr << "[INFO] Original Length: " << original_path.length() << " moves\n";
    cerr << "[INFO] Window Size: " << W << " (Looking for long-range shortcuts!)\n\n";

    string opt_path = original_path;
    bool changed = true;
    int pass = 1;
    int total_removed = 0;
    time_t last_print = time(NULL);

    while (changed) {
        changed = false;
        cerr << "\n[PASS " << pass << "] Scanning...\n";
        string next_path = "";
        vector<int16_t> grid = initial_grid;
        int er = init_er, ec = init_ec;
        int i = 0;

        while (i < opt_path.length()) {
            if (i + W <= opt_path.length()) {
                vector<int16_t> end_grid = grid;
                int end_er = er, end_ec = ec;
                string subpath = opt_path.substr(i, W);
                bool valid = true;
                
                for (char c : subpath) {
                    int ner = end_er, nec = end_ec;
                    if(c=='U') ner++; else if(c=='D') ner--; else if(c=='L') nec++; else if(c=='R') nec--;
                    if(ner < 0 || ner >= N || nec < 0 || nec >= N) { valid = false; break; }
                    swap(end_grid[end_er * N + end_ec], end_grid[ner * N + nec]);
                    end_er = ner; end_ec = nec;
                }

                if (valid) {
                    // โยนเข้า Sniper IDA*
                    string shortcut = find_long_range_shortcut(grid, er, ec, end_grid, W);
                    if (shortcut != "FAILED") {
                        next_path += shortcut;
                        grid = end_grid; er = end_er; ec = end_ec;
                        i += W;
                        changed = true;
                        total_removed += (W - shortcut.length());
                        continue;
                    }
                }
            }

            char c = opt_path[i];
            next_path += c;
            int ner = er, nec = ec;
            if(c=='U') ner++; else if(c=='D') ner--; else if(c=='L') nec++; else if(c=='R') nec--;
            swap(grid[er * N + ec], grid[ner * N + nec]);
            er = ner; ec = nec;
            i++;

            if (time(NULL) - last_print >= 1) {
                last_print = time(NULL);
                cerr << "\rProgress: " << (i * 100 / opt_path.length()) << "% | Sniped Moves: " << total_removed << "      " << flush;
            }
        }
        opt_path = next_path;
        pass++;
    }

    cerr << "\n\n========================================\n";
    cerr << "🎯 [SNIPER SUCCESS] Total Moves Removed: " << total_removed << " moves!\n";
    cerr << "🎯 [SNIPER SUCCESS] Final Length: " << opt_path.length() << " moves\n";
    cerr << "========================================\n";

    string out_filename = string(argv[2]) + "_SNIPED.txt";
    ofstream fout(out_filename);
    fout << opt_path.length() << "\n" << opt_path << "S\n";
    fout.close();

    return 0;
}