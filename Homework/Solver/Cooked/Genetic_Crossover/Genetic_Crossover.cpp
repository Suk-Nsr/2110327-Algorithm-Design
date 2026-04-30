#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <fstream>
#include <cstdint>
#include <random>
#include <algorithm>

using namespace std;

int N;

// 🛡️ ระบบ Zobrist Hashing (เพื่อสแกนหากระดานที่หน้าตาเหมือนกันแบบ O(1))
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

// โครงสร้างกราฟสำหรับการตัดต่อพันธุกรรม
struct Edge {
    int to;
    int weight;
    string moves;
};

// ฟังก์ชันจำลองการเดินเพื่อเก็บ Hash ทุกย่างก้าว
vector<ZobristHash> simulate_and_hash(vector<int16_t> grid, int er, int ec, const string& path) {
    vector<ZobristHash> hashes;
    hashes.reserve(path.length() + 1);
    ZobristHash curr_hash = compute_full_hash(grid);
    hashes.push_back(curr_hash);

    for (char m : path) {
        int ner = er, nec = ec;
        if(m == 'U') ner++; else if(m == 'D') ner--; else if(m == 'L') nec++; else if(m == 'R') nec--;
        
        int v1 = grid[er * N + ec], v2 = grid[ner * N + nec];
        int p1 = er * N + ec, p2 = ner * N + nec;
        
        ZobristHash zh_p1_v1 = get_zobrist(p1, v1), zh_p1_v2 = get_zobrist(p1, v2);
        ZobristHash zh_p2_v2 = get_zobrist(p2, v2), zh_p2_v1 = get_zobrist(p2, v1);
        
        curr_hash.h1 ^= zh_p1_v1.h1 ^ zh_p1_v2.h1 ^ zh_p2_v2.h1 ^ zh_p2_v1.h1;
        curr_hash.h2 ^= zh_p1_v1.h2 ^ zh_p1_v2.h2 ^ zh_p2_v2.h2 ^ zh_p2_v1.h2;
        
        swap(grid[p1], grid[p2]);
        er = ner; ec = nec;
        hashes.push_back(curr_hash);
    }
    return hashes;
}

string read_path(const string& filename) {
    ifstream fsol(filename);
    if (!fsol) { cerr << "[ERROR] Cannot open " << filename << "\n"; exit(1); }
    string original_path = "", temp;
    while (fsol >> temp) {
        string seg = "";
        for (char c : temp) if (c == 'U' || c == 'D' || c == 'L' || c == 'R') seg += c;
        if (seg.length() > original_path.length()) original_path = seg;
    }
    return original_path;
}

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(false); cin.tie(NULL);
    if (argc < 4) {
        cerr << "Usage: ./Genetic_Crossover.exe <puzzle.in> <solution_A.txt> <solution_B.txt>\n";
        return 1;
    }

    // 1. อ่านกระดานเริ่มต้น
    ifstream fin(argv[1]);
    if (!(fin >> N)) return 0;
    init_zobrist();
    
    vector<int16_t> initial_grid(N * N);
    int init_er = -1, init_ec = -1;
    for (int i = 0; i < N * N; ++i) {
        int val; fin >> val; initial_grid[i] = (int16_t)val;
        if (val == -1) { init_er = i / N; init_ec = i % N; }
    }
    fin.close();

    // 2. อ่านไฟล์คำตอบ A และ B
    string pathA = read_path(argv[2]);
    string pathB = read_path(argv[3]);
    
    cerr << "[INFO] Genetic Splicer Initialized...\n";
    cerr << "  - Parent A Length: " << pathA.length() << " moves\n";
    cerr << "  - Parent B Length: " << pathB.length() << " moves\n";

    // 3. จำลองหน้าตากระดานทุกๆ ก้าวของทั้งสองไฟล์
    cerr << "[INFO] Simulating and Mapping DNA sequences...\n";
    vector<ZobristHash> hashesA = simulate_and_hash(initial_grid, init_er, init_ec, pathA);
    vector<ZobristHash> hashesB = simulate_and_hash(initial_grid, init_er, init_ec, pathB);

    // 4. หา "จุดเชื่อมต่อ (Intersection)" ของทั้ง 2 สายพันธุ์
    unordered_map<ZobristHash, int, ZobristHasher> hash_to_id;
    int next_id = 0;
    
    // บังคับให้จุด Start = ID 0 และ Goal = ID 1 เสมอ
    hash_to_id[hashesA[0]] = next_id++;
    hash_to_id[hashesA.back()] = next_id++;

    unordered_set<ZobristHash, ZobristHasher> setA(hashesA.begin(), hashesA.end());
    for (const auto& h : hashesB) {
        if (setA.count(h) && hash_to_id.find(h) == hash_to_id.end()) {
            hash_to_id[h] = next_id++;
        }
    }

    cerr << "[INFO] Found " << next_id << " distinct intersecting DNA nodes!\n";

    // 5. สร้างกราฟเชื่อมโยงรหัสพันธุกรรม
    vector<vector<Edge>> adj(next_id);
    
    auto build_edges = [&](const vector<ZobristHash>& hashes, const string& path) {
        int last_id = hash_to_id[hashes[0]];
        int last_idx = 0;
        for (int i = 1; i < hashes.size(); ++i) {
            auto it = hash_to_id.find(hashes[i]);
            if (it != hash_to_id.end()) {
                int cur_id = it->second;
                adj[last_id].push_back({cur_id, i - last_idx, path.substr(last_idx, i - last_idx)});
                last_id = cur_id;
                last_idx = i;
            }
        }
    };
    
    build_edges(hashesA, pathA);
    build_edges(hashesB, pathB);

    // 6. ใช้ Dijkstra หาเส้นทางผสมพันธุ์ที่สั้นที่สุด
    cerr << "[INFO] Splicing optimal sequence using Dijkstra...\n";
    vector<int> dist(next_id, 2e9);
    vector<int> parent(next_id, -1);
    vector<string> parent_edge(next_id, "");

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    dist[0] = 0;
    pq.push({0, 0});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        if (u == 1) break; // ถึงเป้าหมายแล้ว

        for (auto& edge : adj[u]) {
            int v = edge.to;
            int w = edge.weight;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                parent_edge[v] = edge.moves;
                pq.push({dist[v], v});
            }
        }
    }

    // 7. ประกอบร่างคำตอบใหม่
    if (dist[1] == 2e9) {
        cerr << "[ERROR] Cannot find a valid path to Goal!\n";
        return 1;
    }

    string final_path = "";
    int curr = 1;
    vector<string> segments;
    while (curr != 0) {
        segments.push_back(parent_edge[curr]);
        curr = parent[curr];
    }
    reverse(segments.begin(), segments.end());
    for (const string& s : segments) final_path += s;

    // ระบบขัดเงาครั้งสุดท้าย (ลบการเดินย้อนกลับ)
    bool changed = true;
    while(changed) {
        changed = false;
        string cleaned = "";
        for(char c : final_path) {
            if(!cleaned.empty()) {
                char last = cleaned.back();
                if((last == 'U' && c == 'D') || (last == 'D' && c == 'U') ||
                   (last == 'L' && c == 'R') || (last == 'R' && c == 'L')) {
                    cleaned.pop_back(); changed = true; continue;
                }
            }
            cleaned.push_back(c);
        }
        final_path = cleaned;
    }

    cerr << "\n========================================\n";
    cerr << "🧬 [SUCCESS] SUPER SOLUTION GENERATED!\n";
    cerr << "🧬 Final Spliced Length: " << final_path.length() << " moves\n";
    cerr << "========================================\n";

    string out_filename = "SUPER_CROSSED_" + to_string(final_path.length()) + ".txt";
    ofstream fout(out_filename);
    fout << final_path.length() << "\n" << final_path << "S\n";
    fout.close();

    cerr << "💾 Saved to: " << out_filename << "\n";
    return 0;
}