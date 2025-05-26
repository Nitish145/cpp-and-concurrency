#include <iostream>

template <typename T>
class UniquePtr
{
public:
    UniquePtr(T *p = nullptr) : p(ptr) {}

    ~UniquePtr()
    {
        delete ptr;
    }

    UniquePtr(const UniquePtr<T> &other) = delete;
    UniquePtr &operator=(const UniquePtr<T> &other) = delete;

    UniquePtr(UniquePtr<T> &&other) : ptr(other.release())
    {
    }

    UniquePtr &operator=(UniquePtr<T> &&other)
    {
        if (this != &other)
        {
            delete ptr;
            ptr = other.release();
        }
        return *this;
    }

    void reset(T *other_ptr)
    {
        if (ptr != other_ptr)
        {
            delete ptr;
            ptr = other_ptr;
        }
    }

    T &operator*() { return *ptr; }

    T *operator->() { return ptr; }

private:
    T *release()
    {
        auto released_ptr = ptr;
        ptr = nullptr;
        return released_ptr;
    }

    T *ptr = nullptr;
};

int main()
{
    {
        UniquePtr<int> up1(new int(55));
        std::cout << "up1 points to: " << *up1 << std::endl;

        UniquePtr<int> up2 = std::move(up1);
        std::cout << "After moving up1 to up2:" << std::endl;
        std::cout << "up2 points to: " << *up2 << std::endl;

        up2.reset(new int(77));
        std::cout << "After resetting up2:" << std::endl;
        std::cout << "up2 now points to: " << *up2 << std::endl;
    }

    std::cout << "UniquePtr went out of scope, resource should be released." << std::endl;

    return 0;
}