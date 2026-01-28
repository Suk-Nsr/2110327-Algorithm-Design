#include <iostream>
#include <vector>
#include <algorithm>

// Use long long to prevent overflow (customers up to 10^12)
typedef long long ll;

ll seatsAndTimes, customerNeeds;

// Calculate total customers seated by a specific time
ll calculateTotalCustomerAtTime(ll time, const std::vector<int> & chefTimes)
{
    // Start with the initial N customers seated at time 0
    ll totalCustomer = seatsAndTimes;

    for (int chefTime : chefTimes)
    {
        totalCustomer += (time / chefTime);
    }

    return totalCustomer;
}

int main()
{
    // Optimize I/O operations
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::cin >> seatsAndTimes >> customerNeeds;
    std::vector<int> chefTimes(seatsAndTimes);

    int maxCookTime = 0;
    for (int i = 0; i < seatsAndTimes; ++i)
    {
        std::cin >> chefTimes[i];
        if (chefTimes[i] > maxCookTime) {
            maxCookTime = chefTimes[i];
        }
    }

    for (int i = 0; i < customerNeeds; ++i)
    {
        ll customerPosition;
        std::cin >> customerPosition;

        // Binary Search Logic
        ll low = 0;
        // Upper bound estimation: worst case roughly (customer * min_cook_time) / N
        // 1e15 is sufficiently large given the constraints (10^12 customers * 1000 mins)
        ll high = 1000000000000000LL; 
        ll ans = 0;

        // Standard Lower Bound Binary Search
        while (low <= high)
        {
            ll mid = low + (high - low) / 2;
            
            // Check if we can serve at least 'customerPosition' people by time 'mid'
            if (calculateTotalCustomerAtTime(mid, chefTimes) >= customerPosition)
            {
                ans = mid;      // Possible answer, record it
                high = mid - 1; // Try to find a smaller time
            }
            else
            {
                low = mid + 1;  // Need more time
            }
        }
        
        std::cout << ans << "\n";
    }

    return 0;
}