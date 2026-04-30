#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <cstdint>

using namespace std;

// โครงสร้างแฮช 128-bit
struct Hash128 {
    uint64_t h1, h2;
    bool operator==(const Hash128& o) const { return h1 == o.h1 && h2 == o.h2; }
};

struct Hash128Hasher {
    size_t operator()(const Hash128& k) const {
        return k.h1 ^ (k.h2 << 1);
    }
};

Hash128 compute_hash_128(const vector<int8_t>& v) {
    uint64_t h1 = 14695981039346656037ULL;
    uint64_t h2 = 1099511628211ULL;
    for (size_t i = 0; i < v.size(); ++i) {
        uint64_t val = (uint64_t)v[i] ^ (uint64_t)i; 
        h1 ^= val; h1 *= 1099511628211ULL;
        h2 ^= val; h2 *= 14695981039346656037ULL;
    }
    return {h1, h2};
}

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(false); cin.tie(NULL);
    
    if (argc < 3) {
        cerr << "[ERROR] Usage: ./Compressor.exe <input_board.in> <solution.txt>\n";
        return 1;
    }

    // 1. อ่านกระดานตั้งต้น (บีบอัดให้เป็น int8_t เพื่อเซฟ RAM)
    ifstream fin(argv[1]);
    int N;
    if (!(fin >> N)) return 0;
    vector<int8_t> grid(N * N);
    int er = -1, ec = -1;
    for (int i = 0; i < N * N; ++i) {
        int val;
        fin >> val;
        grid[i] = (int8_t)val;
        if (val == -1) {
            er = i / N;
            ec = i % N;
        }
    }
    fin.close();

    // 2. อ่านไฟล์คำตอบ (ตามที่คุณบอกว่าไฟล์คลีนแล้ว เราก็อ่านตรงๆ ได้เลย)
    ifstream fsol(argv[2]);
    string original_path = "";
    string temp;
    while (fsol >> temp) {
        for (char c : temp) {
            if (c == 'U' || c == 'D' || c == 'L' || c == 'R') {
                original_path += c;
            }
        }
    }
    fsol.close();

    if (original_path.empty()) {
        cerr << "[ERROR] Could not find any valid path in the file!\n";
        return 1;
    }

    cerr << "[INFO] Processing... Original Path Length: " << original_path.length() << " moves\n";

    // 3. เริ่มกระบวนการอัดแท่ง
    string optimized_path = "";
    vector<Hash128> path_hashes;
    vector<vector<int8_t>> path_grids; // 🛡️ THE FIX: เก็บกระดานของจริงไว้เทียบกันชัวร์ๆ
    unordered_map<Hash128, vector<int>, Hash128Hasher> hash_to_indices;

    // จอง RAM ล่วงหน้า ป้องกันโปรแกรมค้าง
    path_hashes.reserve(original_path.length() + 1);
    path_grids.reserve(original_path.length() + 1);
    optimized_path.reserve(original_path.length() + 1);

    Hash128 current_hash = compute_hash_128(grid);
    path_hashes.push_back(current_hash);
    path_grids.push_back(grid);
    hash_to_indices[current_hash].push_back(0);

    int removed_moves = 0;

    for (char move : original_path) {
        int ner = er, nec = ec;
        if (move == 'U') ner++; 
        else if (move == 'D') ner--; 
        else if (move == 'L') nec++; 
        else if (move == 'R') nec--;

        // 🛡️ เช็คความชัวร์ ถ้าต้นฉบับเดินออกนอกกระดาน โวยวายทันที
        if (ner < 0 || ner >= N || nec < 0 || nec >= N) {
            cerr << "\n[FATAL ERROR] The ORIGINAL path walked off the board! Corrupted input file.\n";
            return 1;
        }

        // เดินหมากจำลองบนกระดานจริง
        swap(grid[er * N + ec], grid[ner * N + nec]);
        er = ner;
        ec = nec;

        current_hash = compute_hash_128(grid);
        
        int prev_idx = -1;
        if (hash_to_indices.count(current_hash)) {
            // 🛡️ THE FIX: ถ้าแฮชตรงกัน ต้องเอากระดานมาเทียบกันแบบช่องต่อช่อง!
            for (int idx : hash_to_indices[current_hash]) {
                if (path_grids[idx] == grid) {
                    prev_idx = idx; // เจอการเดินวนลูปของจริงแล้ว!
                    break;
                }
            }
        }

        // ถ้าย้อนกลับมากระดานเดิม (เดินวนลูปของจริง) ให้หั่นก้าวเดินขยะทิ้ง!
        if (prev_idx != -1) {
            int cycle_len = path_hashes.size() - 1 - prev_idx;
            removed_moves += cycle_len + 1;

            // ย้อนเวลากลับไป
            while (path_hashes.size() > prev_idx + 1) {
                Hash128 h = path_hashes.back();
                hash_to_indices[h].pop_back();
                if (hash_to_indices[h].empty()) {
                    hash_to_indices.erase(h);
                }
                path_hashes.pop_back();
                path_grids.pop_back();
                optimized_path.pop_back();
            }
            continue; 
        }

        // ถ้าไม่ลูป ก็เดินปกติ
        optimized_path += move;
        path_hashes.push_back(current_hash);
        path_grids.push_back(grid); // เก็บกระดานไว้เป็นหลักฐาน
        hash_to_indices[current_hash].push_back(optimized_path.length());
    }

    cerr << "\n========================================\n";
    cerr << "🔥 [SUCCESS] Removed " << removed_moves << " redundant moves!\n";
    cerr << "🔥 [SUCCESS] New Optimized Length: " << optimized_path.length() << " moves\n";
    cerr << "========================================\n";

    // 4. บันทึกเป็นไฟล์ใหม่
    string out_filename = string(argv[2]) + "_compressed.txt";
    ofstream fout(out_filename);
    fout << optimized_path.length() << "\n";
    fout << optimized_path << "S\n";
    fout.close();
    
    cerr << "💾 Saved 100% BULLETPROOF solution to: " << out_filename << "\n";

    return 0;
}