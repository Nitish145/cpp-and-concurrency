#include <iostream>
#include <atomic>
#include <vector>
#include <thread>
#include <chrono>

class Spinlock
{
public:
    void lock()
    {
        while (flag.test_and_set(std::memory_order_acquire))
        {
            std::this_thread::yield(); // very much a performance booster
        }
    }

    void unlock()
    {
        flag.clear(std::memory_order_release);
    }

private:
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
};

Spinlock spin;
int counter = 0;

void increment_with_spinlock()
{
    for (int i = 0; i < 1'000'000; ++i)
    {
        spin.lock();
        ++counter;
        spin.unlock();
    }
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i)
        threads.emplace_back(increment_with_spinlock);

    for (auto &t : threads)
        t.join();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Final Counter: " << counter << "\n";
    std::cout << "Time Taken: " << duration.count() << " seconds\n";
}