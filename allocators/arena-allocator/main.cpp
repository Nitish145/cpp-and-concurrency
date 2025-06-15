#include <iostream>

class ArenaAllocator
{
public:
    ArenaAllocator(size_t size) : buffer(nullptr), capacity(size), offset(0)
    {
        buffer = new char[size];
    }

    void *allocate(size_t size, size_t alignment = alignof(std::max_align_t))
    {
        size_t current = reinterpret_cast<size_t>(buffer + offset);
        size_t aligned = (current + alignment - 1) & ~(alignment - 1);
        size_t padding = aligned - current;

        if (offset + padding + size > capacity)
            throw std::bad_alloc();
        offset += padding;
        void *ptr = buffer + offset;
        offset += size;
        return ptr;
    }

    void reset()
    {
        offset = 0;
    }

    ~ArenaAllocator()
    {
        delete[] buffer;
    }

private:
    char *buffer;
    size_t capacity;
    size_t offset;
};

int main()
{
    int size = 1024;
    ArenaAllocator allocator(size);

    // Allocate an int
    int *a = static_cast<int *>(allocator.allocate(sizeof(int)));
    *a = 42;

    // Allocate a double
    double *b = static_cast<double *>(allocator.allocate(sizeof(double)));
    *b = 3.14;

    // Allocate a struct
    struct MyStruct
    {
        int x;
        float y;
    };

    MyStruct *s = static_cast<MyStruct *>(allocator.allocate(sizeof(MyStruct)));
    s->x = 10;
    s->y = 2.5f;

    std::cout << "Int: " << *a << std::endl;
    std::cout << "Double: " << *b << std::endl;
    std::cout << "Struct: { x: " << s->x << ", y: " << s->y << " }" << std::endl;

    // Reset allocator to reuse memory
    allocator.reset();
    std::cout << "Allocator reset. Memory can be reused." << std::endl;
}