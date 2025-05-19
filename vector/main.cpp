#include <iostream>

template <typename T>
class MyVector
{
public:
    MyVector() : data_(nullptr), size_(0), capacity_(0) {}
    MyVector(int capacity) : data_(nullptr), size_(0), capacity_(capacity)
    {
        reallocate(capacity_);
    }

    ~MyVector()
    {
        delete[] data_;
    }

    void push_back(const T &value)
    {
        if (size_ >= capacity_)
            reallocate(capacity_ == 0 ? 1 : capacity_ * 2);
        data_[size_++] = value;
    }

    void pop_back()
    {
        size_--;
    }

    T &operator[](size_t index)
    {
        return data_[index];
    }

    const T &operator[](size_t index) const
    {
        return data_[index];
    }

    size_t size() const
    {
        return size_;
    }

    size_t capacity() const
    {
        return capacity_;
    }

    void reserve(size_t new_capacity)
    {
        if (new_capacity > capacity_)
            reallocate(new_capacity);
    }

    void resize(size_t new_size)
    {
        if (new_size > capacity_)
            reallocate(new_size);
        for (size_t i = size_; i < new_size; ++i)
            data_[i] = T(); // default initialize
        size_ = new_size;
    }

    T *begin() { return data_; }
    T *end() { return data_ + size_; }

private:
    T *data_;
    size_t size_;     // the number of elements stored in the array
    size_t capacity_; // the capacity of the vector

    void reallocate(size_t new_capacity)
    {
        T *new_data = new T[new_capacity];
        for (size_t i = 0; i < size_; i++)
            new_data[i] = data_[i];
        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }
};
