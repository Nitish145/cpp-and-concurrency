#include <iostream>
#include <thread>

using namespace std;

class ThreadPool
{
public:
    using Task = function<void()>;

    ThreadPool(int n = thread::hardware_concurrency()) : m_active(true), m_numThreads(n)
    {
        for (int i = 0; i < m_numThreads; i++)
            this->add_worker();
    }

    ~ThreadPool()
    {
        this->shutdown();
    }

    void shutdown()
    {
        {
            lock_guard<mutex> lock(m_queueMutex);
            m_active = false;
        }
        m_conditionVar.notify_all();
        for (auto &worker : m_workers)
        {
            if (worker.joinable())
                worker.join();
        }
        m_workers.clear();
    }

    void submit_task(Task task)
    {
        lock_guard<mutex> guard(m_queueMutex);
        m_taskQueue.push(task);
        m_conditionVar.notify_one();
    }

    Task pop_task()
    {
        unique_lock<mutex> lock(m_queueMutex);
        m_conditionVar.wait(lock, [&]
                            { return !m_taskQueue.empty() || !m_active; });
        if (!m_active && m_taskQueue.empty())
            return nullptr;
        auto task = m_taskQueue.front();
        m_taskQueue.pop();
        return task;
    }

    void add_worker()
    {
        auto th = thread(
            [this]
            {
                while (true)
                {
                    auto task = pop_task();
                    if (!task)
                        return;
                    task();
                }
            });

        m_workers.push_back(std::move(th));
    }

private:
    int m_numThreads;
    atomic<bool> m_active;
    deque<thread> m_workers;
    queue<Task> m_taskQueue;
    mutex m_queueMutex;
    condition_variable m_conditionVar;
};

int main()
{
    ThreadPool pool(4);

    for (int i = 0; i < 10; ++i)
    {
        pool.submit_task([i]()
                         {
                             cout << "Task " << i << " is being processed by thread "
                                  << this_thread::get_id() << endl;
                             this_thread::sleep_for(chrono::milliseconds(100)); });
    }

    // Give some time for tasks to complete before shutdown
    this_thread::sleep_for(chrono::seconds(2));
    pool.shutdown();
    return 0;
}