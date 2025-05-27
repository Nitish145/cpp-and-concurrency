#include <iostream>

class FixedBlockAllocator
{
public:
    FixedBlockAllocator(size_t blockSize, size_t blockCount)
        : size(blockSize), count(blockCount), buffer(nullptr), freeList(nullptr)
    {
        // consider enabling hugepages allocation for this and divide the memory
        // in smaller chunks as required, the whole page sits inside the TLB
        // and there will no no page faults, great cache locality and super speed.
        buffer = new char[blockSize * blockCount];
        freeList = reinterpret_cast<FreeBlock *>(buffer);

        FreeBlock *block = freeList;
        for (size_t i = 1; i < blockCount; i++)
        {
            block->next = reinterpret_cast<FreeBlock *>(buffer + i * blockSize);
            block = block->next;
        }
        block->next = nullptr;
    }

    void *allocate()
    {
        if (!freeList)
            throw std::bad_alloc();

        void *block = freeList;
        freeList = freeList->next;
        return block;
    }

    void deallocate(void *ptr)
    {
        FreeBlock *block = reinterpret_cast<FreeBlock *>(ptr);
        block->next = freeList;
        freeList = block;
    }

    ~FixedBlockAllocator()
    {
        delete[] buffer;
    }

private:
    struct FreeBlock
    {
        FreeBlock *next;
    };
    size_t size;
    size_t count;
    char *buffer;
    // the address of each free block is the address of memory
    // which is available for allocation
    FreeBlock *freeList;
};

int main()
{
    FixedBlockAllocator fixedAllocator(64, 10);

    void *block1 = fixedAllocator.allocate();
    void *block2 = fixedAllocator.allocate();
    void *block3 = fixedAllocator.allocate();

    std::cout << "Allocated blocks at: " << block1 << ", " << block2 << ", " << block3 << std::endl;

    fixedAllocator.deallocate(block2);
    std::cout << "Deallocated block at: " << block2 << std::endl;

    void *block4 = fixedAllocator.allocate();
    std::cout << "Allocated block at: " << block4 << " (should be same as deallocated block2)" << std::endl;

    fixedAllocator.deallocate(block1);
    fixedAllocator.deallocate(block3);
    fixedAllocator.deallocate(block4);

    std::cout << "All allocated blocks deallocated." << std::endl;
}