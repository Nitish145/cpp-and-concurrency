#include <iostream>
#include <thread>
#include <vector>
#include <atomic>

constexpr int NUM_THREADS = 4;
constexpr int INCREMENTS_PER_THREAD = 1'000'000;

std::atomic<int> counter(0); // Lock-free atomic counter

void increment_with_fetch_add()
{
    for (int i = 0; i < INCREMENTS_PER_THREAD; ++i)
    {
        counter.fetch_add(1, std::memory_order_relaxed); // Fastest, still correct for simple counter
    }
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i)
        threads.emplace_back(increment_with_fetch_add);

    for (auto &t : threads)
        t.join();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Final Counter: " << counter << "\n";
    std::cout << "Time Taken: " << duration.count() << " seconds\n";
}