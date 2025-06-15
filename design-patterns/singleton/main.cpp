#include <memory>
#include <mutex>

using namespace std;

// ================== Singleton using static local variable (Meyers Singleton) =====================
// Thread-safe in C++11 and later.
// Lazy-initialized and preferred approach unless you need custom memory control or manual destruction.
class Singleton
{
public:
    static Singleton &getInstance()
    {
        static Singleton instance;
        return instance;
    }

private:
    Singleton() = default;
};

// ================== Singleton using Double Checked Locking (DCL) =====================
// Manually manages memory using `new`.
// Requires mutex and double-checked locking to be thread-safe.
// Use this only if you need manual control over memory allocation or construction.
class Singleton_DCL
{
public:
    static Singleton_DCL *getInstance()
    {
        if (instance == nullptr) // First check
        {
            lock_guard<mutex> lock(mtx);
            if (instance == nullptr) // Second check
            {
                instance = new Singleton_DCL();
            }
        }
        return instance;
    }

private:
    Singleton_DCL() = default;
    static Singleton_DCL *instance;
    static mutex mtx;
};

Singleton_DCL *Singleton_DCL::instance = nullptr;
mutex Singleton_DCL::mtx;

// ================== Singleton Using CRTP =====================
// Compile-time enforced singleton using CRTP.
// Inherits from a templated base that disables copying and manages instance access.
// Suitable when you want to apply singleton pattern across multiple derived types.
template <typename T>
class Singleton_CRTP
{
public:
    static T &getInstance()
    {
        static T instance;
        return instance;
    }

protected:
    // Constructor and destructor are protected to prevent direct instantiation or destruction,
    // while still allowing derived classes to construct via CRTP base.
    Singleton_CRTP() = default;
    ~Singleton_CRTP() = default;

    Singleton_CRTP(const Singleton_CRTP &) = delete;
    Singleton_CRTP &operator=(const Singleton_CRTP &) = delete;
};

class Logger : public Singleton_CRTP<Logger>
{
private:
    // Grant Singleton_CRTP<Logger> access to Logger's private constructor.
    // This is required so that `static T instance` inside Singleton_CRTP can instantiate Logger.
    friend Singleton_CRTP<Logger>;
    Logger() = default;
};

int main()
{
    // Meyers Singleton usage
    Singleton &s1 = Singleton::getInstance();

    // Double Checked Locking Singleton usage
    Singleton_DCL *s2 = Singleton_DCL::getInstance();
    delete s2;

    // CRTP Singleton usage
    Logger &log = Logger::getInstance();
}