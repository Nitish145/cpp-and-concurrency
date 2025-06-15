#include <iostream>
#include <thread>
#include <vector>

using namespace std;

class KVStore
{
public:
    void Set(const string &key, const string &value)
    {
        lock_guard<mutex> guard(map_mutex);
        auto &lock = locks[key];
        if (!lock)
            locks[key] = make_unique<mutex>();
        lock_guard<mutex> key_guard(*lock);
        kv[key] = value;
    }

    string Get(const string &key)
    {
        lock_guard<mutex> guard(map_mutex);
        if (!locks.count(key) || !locks[key])
            return "";
        lock_guard<mutex> key_guard(*locks[key]);
        return kv[key];
    }

    void Delete(const string &key)
    {
        lock_guard<mutex> guard(map_mutex);
        if (!locks.count(key) || !locks[key])
            return;
        lock_guard<mutex> key_guard(*locks[key]);
        kv.erase(key);
        locks.erase(key);
    }

private:
    unordered_map<string, unique_ptr<mutex>> locks;
    mutex map_mutex;
    unordered_map<string, string> kv;
};

int main()
{
    KVStore store;

    auto writer = [&store](const string &key, const string &value)
    {
        for (int i = 0; i < 5; ++i)
        {
            store.Set(key, value + to_string(i));
            this_thread::sleep_for(chrono::milliseconds(50));
        }
    };

    auto reader = [&store](const string &key)
    {
        for (int i = 0; i < 5; ++i)
        {
            cout << "Read key " << key << ": " << store.Get(key) << endl;
            this_thread::sleep_for(chrono::milliseconds(50));
        }
    };

    thread t1(writer, "alpha", "A");
    thread t2(reader, "alpha");
    thread t3(writer, "beta", "B");
    thread t4(reader, "beta");

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;
}