#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>

template <typename T, size_t SIZE>
class RingBuffer
{
    static_assert((SIZE & (SIZE - 1)) == 0, "SIZE must be power of two");

private:
    T buffer[SIZE];

    alignas(64) std::atomic<size_t> head{0}; // Producer-only
    alignas(64) std::atomic<size_t> tail{0}; // Consumer-only

public:
    bool push(const T &item)
    {
        auto h = head.load(std::memory_order_relaxed);
        auto t = tail.load(std::memory_order_acquire);

        if (h - t >= SIZE)
            return false;

        buffer[h % SIZE] = item;
        head.fetch_add(1, std::memory_order_release);
        return true;
    }

    bool pop(T &item)
    {
        auto t = tail.load(std::memory_order_relaxed);
        auto h = head.load(std::memory_order_acquire);

        if (h == t)
            return false;

        item = buffer[t % SIZE];
        tail.fetch_add(1, std::memory_order_release);
        return true;
    }
};

int main()
{
    constexpr size_t N = 1 << 20; // Ring buffer size (1M)
    constexpr size_t total_messages = 100000000;

    RingBuffer<int, N> rb;

    std::atomic<bool> done{false};
    std::atomic<size_t> produced{0}, consumed{0};

    auto start = std::chrono::high_resolution_clock::now();

    std::thread producer([&]()
                         {
        for (size_t i = 0; i < total_messages; ) {
            if (rb.push(i)) {
                ++i;
                ++produced;
            }
        } });

    std::thread consumer([&]()
                         {
        size_t expected = 0;
        int val;
        while (consumed < total_messages) {
            if (rb.pop(val)) {
                assert(val == expected);  // Validate order
                ++expected;
                ++consumed;
            }
        }
        done = true; });

    producer.join();
    consumer.join();

    auto end = std::chrono::high_resolution_clock::now();
    double seconds = std::chrono::duration<double>(end - start).count();

    std::cout << "Processed " << total_messages << " messages in "
              << seconds << " seconds (" << (total_messages / seconds)
              << " ops/sec)\n";

    return 0;
}