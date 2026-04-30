#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <random>

using namespace std;
using namespace chrono;

static int N, M;
static const int MAXN = 70;
static const int DR[4] = { 1, -1, 0, 0 };
static const int DC[4] = { 0, 0, 1, -1 };
static const char DCH[4] = { 'U', 'D', 'L', 'R' };
static const int OPP[4] = { 1, 0, 3, 2 };
static int dirOf(char c){ return c=='U'?0:c=='D'?1:c=='L'?2:3; }

static int init_board[MAXN][MAXN];
static int tgt[MAXN][MAXN];
static int init_br, init_bc;

static auto t_start = steady_clock::now();
static double elapsed(){ return duration<double>(steady_clock::now() - t_start).count(); }

static string g_out_prefix;
static mutex g_log_mutex;
static void logMsg(const string& s){
    lock_guard<mutex> lk(g_log_mutex);
    cerr << "[t=" << (int)elapsed() << "s] " << s << "\n";
}

// ลบการเดินกลับไปกลับมา
static string stripInversePairs(string S){
    bool changed = true;
    while (changed){
        changed = false;
        string out; out.reserve(S.size());
        for (char c : S){
            if (!out.empty()){
                int d1 = dirOf(out.back()), d2 = dirOf(c);
                if (d1 == OPP[d2]){ out.pop_back(); changed = true; continue; }
            }
            out.push_back(c);
        }
        S = std::move(out);
    }
    return S;
}

// ระบบบันทึกประวัติ O(1)
struct Hist {
    int K = 500;
    vector<vector<short>> cks;
    vector<int> blks;
    const string* S = nullptr;

    void build(const string& s){
        S = &s;
        int n = (int)s.size() / K + 1;
        cks.assign(n, {});
        blks.assign(n, 0);
        vector<short> b(N*N);
        for (int r=0; r<N; r++) for (int c=0; c<N; c++) b[r*N+c] = (short)init_board[r][c];
        int blank = init_br * N + init_bc;
        cks[0] = b; blks[0] = blank;
        int ck = 1;
        for (int i = 0; i < (int)s.size(); i++){
            int d = dirOf(s[i]);
            int r = blank / N, c = blank % N;
            int nr = r + DR[d], nc = c + DC[d];
            swap(b[r*N+c], b[nr*N+nc]);
            blank = nr*N+nc;
            if ((i+1) % K == 0 && ck < n){ cks[ck] = b; blks[ck] = blank; ck++; }
        }
    }
    void at(int i, short* out, int& out_blank) const {
        int ck = i / K; if (ck >= (int)cks.size()) ck = (int)cks.size()-1;
        memcpy(out, cks[ck].data(), N*N*sizeof(short));
        int blank = blks[ck];
        for (int j = ck * K; j < i; j++){
            int d = dirOf((*S)[j]);
            int r = blank/N, c = blank%N;
            int nr = r+DR[d], nc = c+DC[d];
            swap(out[r*N+c], out[nr*N+nc]);
            blank = nr*N+nc;
        }
        out_blank = blank;
    }
};

// ============================================================
// THE O(1) HYPER IDA* ENGINE
// ============================================================
struct IDA {
    vector<int> tgt_r, tgt_c;
    vector<int> cur_r, cur_c;
    vector<short> board;
    vector<int> active_targets; // เก็บเฉพาะ ID ที่หน้าต่างนี้ต้องการขยับ
    int blank_r, blank_c;
    long long nodes = 0, cap = 0;
    string path;
};

// คำนวณครั้งแรกครั้งเดียวเท่านั้น (รันเร็วปรี๊ด)
static int computeH0(IDA& I){
    int h = 0;
    for (int id : I.active_targets){
        h += abs(I.cur_r[id] - I.tgt_r[id]) + abs(I.cur_c[id] - I.tgt_c[id]);
    }
    return h;
}

// ⚡ ความเร็วแสง: อัปเดต h แบบ O(1) โดยไม่ใช้ Loop
static bool dfs(IDA& I, int last_dir, int g, int h, int bound, int K){
    I.nodes++;
    if ((I.nodes & 0xFFFFF) == 0 && I.nodes >= I.cap) return false;
    if (g + h > bound) return false;
    if (h == 0) return true; // ทะลุถึงเป้าแล้ว
    if (g >= K) return false;

    for (int d = 0; d < 4; d++){
        if (last_dir >= 0 && d == OPP[last_dir]) continue;
        int nr = I.blank_r + DR[d], nc = I.blank_c + DC[d];
        if (nr<0||nr>=N||nc<0||nc>=N) continue;

        int moved_id = I.board[nr*N+nc];
        int delta_h = 0;
        
        // คำนวณความต่างแค่แผ่นเดียว O(1)
        if (moved_id > 0 && I.tgt_r[moved_id] >= 0) {
            int old_dist = abs(nr - I.tgt_r[moved_id]) + abs(nc - I.tgt_c[moved_id]);
            int new_dist = abs(I.blank_r - I.tgt_r[moved_id]) + abs(I.blank_c - I.tgt_c[moved_id]);
            delta_h = new_dist - old_dist;
        }

        swap(I.board[I.blank_r*N+I.blank_c], I.board[nr*N+nc]);
        if (moved_id > 0){ I.cur_r[moved_id] = I.blank_r; I.cur_c[moved_id] = I.blank_c; }
        
        int pr = I.blank_r, pc = I.blank_c;
        I.blank_r = nr; I.blank_c = nc;
        I.path.push_back(DCH[d]);

        // โยน h ใหม่เข้าไปเลย
        if (dfs(I, d, g+1, h + delta_h, bound, K)) return true;

        I.path.pop_back();
        I.blank_r = pr; I.blank_c = pc;
        swap(I.board[I.blank_r*N+I.blank_c], I.board[nr*N+nc]);
        if (moved_id > 0){ I.cur_r[moved_id] = nr; I.cur_c[moved_id] = nc; }
    }
    return false;
}

static int tryWindow(IDA& I, const short* start_board, int start_blank, const short* end_board, int K, long long node_cap, string& new_seq){
    int ids_max = N * N + 4;
    I.tgt_r.assign(ids_max, -1); I.tgt_c.assign(ids_max, -1);
    I.cur_r.assign(ids_max, -1); I.cur_c.assign(ids_max, -1);
    I.board.assign(N*N, 0);
    I.active_targets.clear();

    for (int r=0; r<N; r++) for (int c=0; c<N; c++){
        short v = start_board[r*N+c];
        I.board[r*N+c] = v;
        if (v > 0){ I.cur_r[v] = r; I.cur_c[v] = c; }
    }
    int changed = 0;
    for (int r=0; r<N; r++) for (int c=0; c<N; c++){
        short v = end_board[r*N+c];
        if (v <= 0) continue;
        if (I.cur_r[v] != r || I.cur_c[v] != c){
            I.tgt_r[v] = r; I.tgt_c[v] = c; changed++;
            I.active_targets.push_back(v); // เก็บเฉพาะแผ่นที่ต้องขยับ
        }
    }
    if (changed == 0){ new_seq = ""; return K; }

    I.blank_r = start_blank / N; I.blank_c = start_blank % N;
    I.nodes = 0; I.cap = node_cap; I.path.clear();

    int h0 = computeH0(I);
    if (h0 >= K) return 0; // ถ้าแมนฮัตตันประเมินว่าเกินระยะหน้าต่าง ตัดทิ้งเลย!

    for (int bound = h0; bound < K; bound++){
        I.nodes = 0; I.path.clear();
        if (dfs(I, -1, 0, h0, bound, K)){ // ผ่าน h0 เข้าไปแทน
            new_seq = I.path;
            return K - (int)new_seq.size();
        }
        if (I.nodes >= I.cap) return 0;
    }
    return 0;
}

// ============================================================
// MULTI-THREADING (16 แกนสมอง ปูพรมสแกน)
// ============================================================
static mutex g_S_mutex;
static string g_best_S;

struct SweepArgs {
    int thread_id;
    vector<int> window_sizes;
    long long node_cap_per_window;
};

static void sweepThread(SweepArgs A){
    IDA I;
    int pass = 0;
    while (true){
        pass++;
        g_S_mutex.lock(); string S = g_best_S; g_S_mutex.unlock();
        Hist H; H.build(S);
        int K = A.window_sizes[pass % A.window_sizes.size()];
        if ((int)S.size() <= K) continue;

        long long saved_this_pass = 0;
        vector<short> bA(N*N), bB(N*N);
        int blA, blB;
        int sz = (int)S.size();
        
        for (int i = 0; i < sz - K; i++){
            H.at(i, bA.data(), blA);
            H.at(i + K, bB.data(), blB);

            string repl;
            int saved = tryWindow(I, bA.data(), blA, bB.data(), K, A.node_cap_per_window, repl);
            if (saved > 0){
                g_S_mutex.lock();
                if ((int)g_best_S.size() >= i + K && g_best_S.substr(0, i + K) == S.substr(0, i + K)){
                    string newS = g_best_S.substr(0, i) + repl + g_best_S.substr(i + K);
                    newS = stripInversePairs(newS);
                    g_best_S = newS;
                    saved_this_pass += saved;
                    logMsg("[SWEEPER T" + to_string(A.thread_id) + " | W=" + to_string(K) + "] SNIPED -" + to_string(saved) + " moves! >> Best: " + to_string(g_best_S.size()));
                    
                    S = g_best_S; H.build(S); sz = (int)S.size();
                    i = max(0, i - K/2);
                }
                g_S_mutex.unlock();
            }
        }
        if (saved_this_pass == 0 && pass > A.window_sizes.size()) break;
    }
}

static void randomSAThread(SweepArgs A){
    mt19937 rng((uint32_t)time(nullptr) ^ (uint32_t)(A.thread_id * 7919));
    IDA I;
    while (true){
        g_S_mutex.lock(); string S = g_best_S; g_S_mutex.unlock();
        Hist H; H.build(S);
        int sz = (int)S.size();
        if (sz < 50) break;

        int K = A.window_sizes[rng() % A.window_sizes.size()];
        if (sz <= K) continue;

        vector<short> bA(N*N), bB(N*N);
        int blA, blB;
        int iters = 1000;
        
        for (int it = 0; it < iters; it++){
            int i = rng() % (sz - K);
            H.at(i, bA.data(), blA);
            H.at(i + K, bB.data(), blB);
            string repl;
            int saved = tryWindow(I, bA.data(), blA, bB.data(), K, A.node_cap_per_window, repl);
            if (saved > 0){
                g_S_mutex.lock();
                if ((int)g_best_S.size() >= i + K && g_best_S.substr(0, i + K) == S.substr(0, i + K)){
                    string newS = g_best_S.substr(0, i) + repl + g_best_S.substr(i + K);
                    g_best_S = stripInversePairs(newS);
                    logMsg("[SA SNIPER T" + to_string(A.thread_id) + " | W=" + to_string(K) + "] SNIPED -" + to_string(saved) + " moves! >> Best: " + to_string(g_best_S.size()));
                    S = g_best_S; H.build(S); sz = (int)S.size();
                }
                g_S_mutex.unlock();
            }
        }
    }
}

int main(int argc, char** argv){
    if (argc < 3){
        cerr << "Usage: ./L_Compressor.exe <puzzle.in> <solution.txt>\n";
        return 1;
    }
    g_out_prefix = string(argv[2]) + "_COMPRESSED";

    ifstream fin(argv[1]);
    fin >> N; M = N - 2;
    for (int r=0; r<N; r++) for (int c=0; c<N; c++){
        fin >> init_board[r][c];
        if (init_board[r][c] == -1){ init_br = r; init_bc = c; init_board[r][c] = 0; }
    }
    for (int r=0; r<M; r++) for (int c=0; c<M; c++) fin >> tgt[r][c];
    fin.close();

    ifstream fsol(argv[2]);
    string temp;
    while (fsol >> temp) {
        string seg = "";
        for (char c : temp) if (c == 'U' || c == 'D' || c == 'L' || c == 'R') seg += c;
        if (seg.length() > g_best_S.length()) g_best_S = seg;
    }
    fsol.close();
    
    g_best_S = stripInversePairs(g_best_S);
    logMsg("===== O(1) HYPER SWEEPER ENGINE =====");
    logMsg("Loaded Original Length: " + to_string(g_best_S.size()) + " moves");

    vector<thread> ths;
    vector<SweepArgs> args(16);
    
    // 🎯 ความสมจริง: หน้าต่างเล็ก รันไวเจอทางลัดง่าย หน้าต่างกลางเผื่อเหนียว (ไม่เกิน 100 ก้าว)
    args[0] = { 0, {8, 10, 12},     1000000LL };
    args[1] = { 1, {14, 16, 18},    2000000LL };
    args[2] = { 2, {20, 24, 28},    5000000LL };
    args[3] = { 3, {30, 36, 42},   10000000LL };
    args[4] = { 4, {46, 52, 60},   20000000LL };
    args[5] = { 5, {64, 70, 76},   50000000LL };
    args[6] = { 6, {80, 86, 94},  100000000LL };
    args[7] = { 7, {98, 104, 110},200000000LL }; 

    for(int i=8; i<16; i++) {
        args[i] = { i, {20, 30, 50, 70, 90}, 50000000LL };
        ths.emplace_back(randomSAThread, args[i]);
    }
    for (int i=0; i<8; i++) ths.emplace_back(sweepThread, args[i]);
    
    atomic<bool> done{false};
    thread saver([&](){
        while(!done){
            this_thread::sleep_for(chrono::seconds(60));
            g_S_mutex.lock(); string snap = g_best_S; g_S_mutex.unlock();
            ofstream fout(g_out_prefix + "_" + to_string(snap.size()) + ".txt");
            fout << snap.size() << "\n" << snap << "S\n"; fout.close();
            logMsg("[AUTO-SAVE] Progress saved! Current length: " + to_string(snap.size()));
        }
    });

    for (auto& t : ths) t.join();
    done = true; saver.join();

    ofstream fout(g_out_prefix + "_FINAL.txt");
    fout << g_best_S.size() << "\n" << g_best_S << "S\n"; fout.close();

    logMsg("===== COMPRESSION DONE! =====");
    return 0;
}