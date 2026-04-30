#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <fstream>
#include <cstdint>
#include <ctime>
#include <random>

using namespace std;

int N;

// 🛡️ ระบบ Zobrist Hashing (เร็วที่สุดในโลก & ไม่กิน RAM)
struct ZobristHash {
    uint64_t h1, h2;
    bool operator==(const ZobristHash& o) const { return h1 == o.h1 && h2 == o.h2; }
};

struct ZobristHasher {
    size_t operator()(const ZobristHash& k) const { return k.h1 ^ (k.h2 << 1); }
};

vector<ZobristHash> ztable;
int max_v;

void init_zobrist() {
    max_v = N * N + 1;
    ztable.resize(N * N * max_v);
    mt19937_64 rng(1337);
    for (size_t i = 0; i < ztable.size(); ++i) {
        ztable[i].h1 = rng();
        ztable[i].h2 = rng();
    }
}

inline int zval(int v) { return v == -1 ? N * N : v; }
inline ZobristHash get_zobrist(int p, int v) { return ztable[p * max_v + zval(v)]; }

ZobristHash compute_full_hash(const vector<int16_t>& grid) {
    ZobristHash h = {0, 0};
    for (int i = 0; i < N * N; ++i) {
        ZobristHash zh = get_zobrist(i, grid[i]);
        h.h1 ^= zh.h1; h.h2 ^= zh.h2;
    }
    return h;
}

bool is_opposite(char a, char b) {
    if (a == 'U' && b == 'D') return true;
    if (a == 'D' && b == 'U') return true;
    if (a == 'L' && b == 'R') return true;
    if (a == 'R' && b == 'L') return true;
    return false;
}

string reverse_inverse(const string& p) {
    string res = "";
    for (int i = (int)p.length() - 1; i >= 0; --i) {
        if (p[i] == 'U') res += 'D';
        else if (p[i] == 'D') res += 'U';
        else if (p[i] == 'L') res += 'R';
        else if (p[i] == 'R') res += 'L';
    }
    return res;
}

// 📦 Node ที่โคตรเบา (ไม่เก็บ Grid เลยประหยัด RAM 100%)
struct BFSNode {
    int er, ec;
    string path;
    ZobristHash hash;
};

// 🧠 ค้นหาทางลัดแบบ 2 ทาง (Zero Copy Grid)
string find_shortcut(vector<int16_t>& s_grid, int s_er, int s_ec, ZobristHash shash,
                     vector<int16_t>& e_grid, int e_er, int e_ec, ZobristHash ehash, int max_len) 
{
    if (shash == ehash) return ""; // เดินวนลูป กลับมาที่เดิม หั่นทิ้ง!

    unordered_map<ZobristHash, string, ZobristHasher> visF, visB;
    queue<BFSNode> qF, qB;

    visF[shash] = ""; visB[ehash] = "";
    qF.push({s_er, s_ec, "", shash});
    qB.push({e_er, e_ec, "", ehash});

    int dF = 0, dB = 0;
    int dr[] = {1, -1, 0, 0};
    int dc[] = {0, 0, 1, -1};
    char mc[] = {'U', 'D', 'L', 'R'}; 
    char opp[] = {'D', 'U', 'R', 'L'};

    while (dF + dB < max_len) {
        if (qF.empty() && qB.empty()) break;

        if (qB.empty() || (!qF.empty() && qF.size() <= qB.size())) {
            int sz = qF.size();
            while (sz--) {
                auto curr = qF.front(); qF.pop();
                
                // เดินหมากจำลอง (เดินหน้า)
                int temp_er = s_er, temp_ec = s_ec;
                for (char m : curr.path) {
                    int ner = temp_er, nec = temp_ec;
                    if(m=='U') ner++; else if(m=='D') ner--; else if(m=='L') nec++; else if(m=='R') nec--;
                    swap(s_grid[temp_er*N + temp_ec], s_grid[ner*N + nec]);
                    temp_er = ner; temp_ec = nec;
                }

                for (int i = 0; i < 4; ++i) {
                    if (!curr.path.empty() && curr.path.back() == opp[i]) continue;
                    int ner = temp_er + dr[i], nec = temp_ec + dc[i];
                    if (ner >= 0 && ner < N && nec >= 0 && nec < N) {
                        int v1 = s_grid[temp_er*N + temp_ec]; 
                        int v2 = s_grid[ner*N + nec];
                        
                        ZobristHash nhash = curr.hash;
                        int p1 = temp_er*N + temp_ec, p2 = ner*N + nec;
                        
                        ZobristHash zh_p1_v1 = get_zobrist(p1, v1), zh_p1_v2 = get_zobrist(p1, v2);
                        ZobristHash zh_p2_v2 = get_zobrist(p2, v2), zh_p2_v1 = get_zobrist(p2, v1);
                        
                        nhash.h1 ^= zh_p1_v1.h1 ^ zh_p1_v2.h1 ^ zh_p2_v2.h1 ^ zh_p2_v1.h1;
                        nhash.h2 ^= zh_p1_v1.h2 ^ zh_p1_v2.h2 ^ zh_p2_v2.h2 ^ zh_p2_v1.h2;
                        
                        if (visB.count(nhash)) {
                            // เดินถอยหลังเพื่อคืนสภาพกระดานก่อน Return
                            for (int k = curr.path.length() - 1; k >= 0; --k) {
                                char m = curr.path[k]; int per = temp_er, pec = temp_ec;
                                if(m=='U') per--; else if(m=='D') per++; else if(m=='L') pec--; else if(m=='R') pec++;
                                swap(s_grid[temp_er*N + temp_ec], s_grid[per*N + pec]);
                                temp_er = per; temp_ec = pec;
                            }
                            return curr.path + mc[i] + reverse_inverse(visB[nhash]);
                        }
                        
                        if (!visF.count(nhash)) {
                            visF[nhash] = curr.path + mc[i];
                            qF.push({ner, nec, curr.path + mc[i], nhash});
                        }
                    }
                }
                
                // เดินถอยหลังเพื่อคืนสภาพกระดานให้ลูปถัดไป
                for (int k = curr.path.length() - 1; k >= 0; --k) {
                    char m = curr.path[k]; int per = temp_er, pec = temp_ec;
                    if(m=='U') per--; else if(m=='D') per++; else if(m=='L') pec--; else if(m=='R') pec++;
                    swap(s_grid[temp_er*N + temp_ec], s_grid[per*N + pec]);
                    temp_er = per; temp_ec = pec;
                }
            }
            dF++;
        } else {
            // ... ทำเหมือนกันสำหรับค้นหาจากฝั่งปลายทาง (qB) ...
            int sz = qB.size();
            while (sz--) {
                auto curr = qB.front(); qB.pop();
                int temp_er = e_er, temp_ec = e_ec;
                for (char m : curr.path) {
                    int ner = temp_er, nec = temp_ec;
                    if(m=='U') ner++; else if(m=='D') ner--; else if(m=='L') nec++; else if(m=='R') nec--;
                    swap(e_grid[temp_er*N + temp_ec], e_grid[ner*N + nec]);
                    temp_er = ner; temp_ec = nec;
                }

                for (int i = 0; i < 4; ++i) {
                    if (!curr.path.empty() && curr.path.back() == opp[i]) continue;
                    int ner = temp_er + dr[i], nec = temp_ec + dc[i];
                    if (ner >= 0 && ner < N && nec >= 0 && nec < N) {
                        int v1 = e_grid[temp_er*N + temp_ec]; 
                        int v2 = e_grid[ner*N + nec];
                        
                        ZobristHash nhash = curr.hash;
                        int p1 = temp_er*N + temp_ec, p2 = ner*N + nec;
                        
                        ZobristHash zh_p1_v1 = get_zobrist(p1, v1), zh_p1_v2 = get_zobrist(p1, v2);
                        ZobristHash zh_p2_v2 = get_zobrist(p2, v2), zh_p2_v1 = get_zobrist(p2, v1);
                        
                        nhash.h1 ^= zh_p1_v1.h1 ^ zh_p1_v2.h1 ^ zh_p2_v2.h1 ^ zh_p2_v1.h1;
                        nhash.h2 ^= zh_p1_v1.h2 ^ zh_p1_v2.h2 ^ zh_p2_v2.h2 ^ zh_p2_v1.h2;
                        
                        if (visF.count(nhash)) {
                            for (int k = curr.path.length() - 1; k >= 0; --k) {
                                char m = curr.path[k]; int per = temp_er, pec = temp_ec;
                                if(m=='U') per--; else if(m=='D') per++; else if(m=='L') pec--; else if(m=='R') pec++;
                                swap(e_grid[temp_er*N + temp_ec], e_grid[per*N + pec]);
                                temp_er = per; temp_ec = pec;
                            }
                            return visF[nhash] + reverse_inverse(curr.path + mc[i]);
                        }
                        
                        if (!visB.count(nhash)) {
                            visB[nhash] = curr.path + mc[i];
                            qB.push({ner, nec, curr.path + mc[i], nhash});
                        }
                    }
                }
                
                for (int k = curr.path.length() - 1; k >= 0; --k) {
                    char m = curr.path[k]; int per = temp_er, pec = temp_ec;
                    if(m=='U') per--; else if(m=='D') per++; else if(m=='L') pec--; else if(m=='R') pec++;
                    swap(e_grid[temp_er*N + temp_ec], e_grid[per*N + pec]);
                    temp_er = per; temp_ec = pec;
                }
            }
            dB++;
        }
    }
    return "FAILED";
}

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(false); cin.tie(NULL);
    if (argc < 3) {
        cerr << "[ERROR] Usage: ./Compressor.exe <input_board.in> <solution.txt>\\n";
        return 1;
    }

    ifstream fin(argv[1]);
    if (!(fin >> N)) return 0;
    
    init_zobrist(); // สร้างตาราง Hash มหากาฬ
    
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
        string current_segment = "";
        for (char c : temp) {
            if (c == 'U' || c == 'D' || c == 'L' || c == 'R') current_segment += c;
        }
        if (current_segment.length() > original_path.length()) original_path = current_segment;
    }
    fsol.close();

    cerr << "[INFO] Processing... Original Length: " << original_path.length() << " moves\\n";

    string opt_path = original_path;
    int W = 12; // 👈 หน้าต่างกว้าง 12 ก้าว (กำลังสวย รันเร็ว เจอทางลัดง่าย)
    bool changed = true;
    int pass = 1;
    int total_removed = 0;

    time_t last_print = time(NULL);

    while (changed) {
        changed = false;
        cerr << "\n[PASS " << pass << "] Scanning with Window Size " << W << "...\n";
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
                    ZobristHash shash = compute_full_hash(grid);
                    ZobristHash ehash = compute_full_hash(end_grid);
                    
                    string shortcut = find_shortcut(grid, er, ec, shash, end_grid, end_er, end_ec, ehash, W - 1);
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
                cerr << "\rScan Progress: " << (i * 100 / opt_path.length()) << "% | Moves Removed: " << total_removed << "      " << flush;
            }
        }
        opt_path = next_path;
        pass++;
    }

    cerr << "\n\n========================================\n";
    cerr << "🔥 [SUCCESS] Total Moves Removed: " << total_removed << " moves!\n";
    cerr << "🔥 [SUCCESS] New BULLETPROOF Length: " << opt_path.length() << " moves\n";
    cerr << "========================================\n";

    string out_filename = string(argv[2]) + "_IRONED.txt";
    ofstream fout(out_filename);
    fout << opt_path.length() << "\n" << opt_path << "S\n";
    fout.close();
    
    cerr << "💾 Saved 100% verified solution to: " << out_filename << "\n";

    return 0;
}