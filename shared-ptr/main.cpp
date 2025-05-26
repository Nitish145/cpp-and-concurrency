#include <iostream>

using namespace std;

template <typename T>
class SharedPointer
{
public:
    SharedPointer() : ptr(nullptr), ref_count(nullptr) {}
    explicit SharedPointer(T *p) : ptr(p), ref_count(new atomic<int>(1)) {}

    ~SharedPointer()
    {
        release();
    };

    SharedPointer(const SharedPointer &other) : ptr(other.ptr),
                                                ref_count(other.ref_count)
    {
        if (ref_count)
            ++(*ref_count);
    }

    SharedPointer(SharedPointer &&other) : ptr(other.ptr), ref_count(other.ref_count)
    {
        // no increment in the ref count
        other.ptr = nullptr;
        other.ref_count = nullptr;
    }

    SharedPointer &operator=(const SharedPointer &other)
    {
        if (this != &other)
        {
            release();
            ptr = other.ptr;
            ref_count = other.ref_count;
            if (ref_count)
                ++(*ref_count);
        }
        return *this;
    }

    SharedPointer &operator=(SharedPointer &&other)
    {
        if (this != &other)
        {
            release();
            ptr = other.ptr;
            ref_count = other.ref_count;
            other.ptr = nullptr;
            other.ref_count = nullptr;
        }
        return *this;
    }

    void release()
    {
        if (ref_count)
        {
            if (--(*ref_count) == 0)
            {
                delete ref_count;
                delete ptr;
            }
        }
    }

    T &operator*()
    {
        return *ptr;
    }

    T *operator->()
    {
        return ptr;
    }

    int count()
    {
        return *ref_count;
    }

private:
    atomic<int> *ref_count;
    T *ptr;
};

int main()
{
    {
        SharedPointer<int> sp1(new int(100));
        std::cout << "sp1 value: " << *sp1 << ", ref_count: " << sp1.count() << std::endl;

        SharedPointer<int> sp2 = sp1;
        std::cout << "After copying sp1 to sp2:" << std::endl;
        std::cout << "sp1 ref_count: " << sp1.count() << ", sp2 ref_count: " << sp2.count() << std::endl;

        SharedPointer<int> sp3 = std::move(sp1);
        std::cout << "After moving sp1 to sp3:" << std::endl;
        std::cout << "sp3 value: " << *sp3 << ", ref_count: " << sp3.count() << std::endl;

        // sp1 is now empty, so we avoid dereferencing it
        std::cout << "sp2 ref_count (should still be valid): " << sp2.count() << std::endl;
    }

    std::cout << "SharedPointers went out of scope, resources should be released." << std::endl;

    return 0;
}