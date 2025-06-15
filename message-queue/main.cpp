#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <optional>

using namespace std;

template <typename T>
class MessageQueue
{
public:
    void push(T item)
    {
        lock_guard<mutex> guard(m_qmutex);
        m_queue.push(item);
        m_qvar.notify_one();
    }

    T pop()
    {
        auto lock = unique_lock<mutex>(m_qmutex);
        m_qvar.wait(lock, [this]
                    { return !this->m_queue.empty(); });
        auto val = m_queue.front();
        m_queue.pop();
        return val;
    }

private:
    queue<T> m_queue;
    mutex m_qmutex;
    condition_variable m_qvar;
};

int main()
{
    MessageQueue<int> mq;

    // Producer thread
    thread producer([&mq]()
                    {
        for (int i = 0; i < 5; ++i) {
            cout << "Producing: " << i << endl;
            mq.push(i);
            this_thread::sleep_for(chrono::milliseconds(100));
        } });

    // Consumer thread
    thread consumer([&mq]()
                    {
        for (int i = 0; i < 5; ++i) {
            int item = mq.pop();
            cout << "Consumed: " << item << endl;
        } });

    producer.join();
    consumer.join();

    return 0;
}