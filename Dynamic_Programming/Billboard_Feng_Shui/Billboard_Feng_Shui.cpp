#include <iostream>
#include <vector>
#include <algorithm> // สำหรับใช้งาน std::max

int main()
{
    int roadLength, constraintLength;
    if (!(std::cin >> roadLength >> constraintLength)) return 0;

    std::vector<int> customerSee(roadLength);
    for (int i = 0; i < roadLength; ++i)
    {
        std::cin >> customerSee[i];
    }

    std::vector<int> constraintList(constraintLength);
    for (int i = 0; i < constraintLength; ++i)
    {
        std::cin >> constraintList[i];
    }

    // ---------------------------------------------------------
    // ส่วนที่ 1: สร้างตาราง DFA (next_state) ด้วย Vector
    // ---------------------------------------------------------
    std::vector<std::vector<int>> next_state(constraintLength, std::vector<int>(2, 0));
    
    for (int j = 0; j < constraintLength; ++j) {
        for (int c = 0; c <= 1; ++c) {
            std::vector<int> current_seq;
            for (int k = 0; k < j; ++k) {
                current_seq.push_back(constraintList[k]);
            }
            current_seq.push_back(c);

            int current_len = current_seq.size();
            int max_match = 0;

            for (int len = current_len; len >= 0; --len) {
                bool is_match = true;
                for (int k = 0; k < len; ++k) {
                    if (current_seq[current_len - len + k] != constraintList[k]) {
                        is_match = false;
                        break;
                    }
                }
                if (is_match) {
                    max_match = len;
                    break;
                }
            }
            next_state[j][c] = max_match;
        }
    }

    // ---------------------------------------------------------
    // ส่วนที่ 2: สร้างและประมวลผลตาราง DP
    // ---------------------------------------------------------
    std::vector<std::vector<std::vector<long long>>> dp(
        roadLength + 1, 
        std::vector<std::vector<long long>>(
            constraintLength + 1, 
            std::vector<long long>(2, -1)
        )
    );
    
    dp[0][0][0] = 0; 

    for (int i = 0; i < roadLength; ++i) {
        for (int j = 0; j < constraintLength; ++j) {
            for (int last = 0; last <= 1; ++last) {
                
                if (dp[i][j][last] == -1) continue;

                // ทางเลือกที่ 1: "ไม่ติดป้ายโฆษณา" (c = 0)
                int nj_0 = next_state[j][0];
                if (nj_0 < constraintLength) { 
                    dp[i + 1][nj_0][0] = std::max(dp[i + 1][nj_0][0], dp[i][j][last]);
                }

                // ทางเลือกที่ 2: "ติดป้ายโฆษณา" (c = 1)
                if (last == 0) { 
                    int nj_1 = next_state[j][1];
                    if (nj_1 < constraintLength) { 
                        dp[i + 1][nj_1][1] = std::max(dp[i + 1][nj_1][1], dp[i][j][last] + customerSee[i]);
                    }
                }
            }
        }
    }

    // ---------------------------------------------------------
    // ส่วนที่ 3: สรุปผลหาค่าที่มากที่สุด
    // ---------------------------------------------------------
    long long max_audience = 0;
    for (int j = 0; j < constraintLength; ++j) {
        for (int last = 0; last <= 1; ++last) {
            max_audience = std::max(max_audience, dp[roadLength][j][last]);
        }
    }

    std::cout << max_audience << "\n";

    return 0;
}