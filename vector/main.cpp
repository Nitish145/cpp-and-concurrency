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

    MyVector(const MyVector<T> &other) : size_(other.size_), capacity_(other.capacity_)
    {
        data_ = new T[capacity_];
        for (int i = 0; i < size_; i++)
            data_[i] = other.data_[i];
    }

    MyVector &operator=(const MyVector<T> &other)
    {
        // important so that we don't accidentlally delete data on self-assignment
        if (this != &other)
        {
            delete[] data_;
            data_ = new T[other.capacity_];
            size_ = other.size_;
            capacity_ = other.capacity_;
            for (int i = 0; i < size_; i++)
                data_[i] = other.data_[i];
        }
        return *this;
    }

    MyVector(MyVector &&other) : data_(other.data_), size_(other.size_), capacity_(other.capacity_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    MyVector &operator=(MyVector &&other)
    {
        if (this != &other)
        {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    void push_back(const T &value)
    {
        emplace_back(value);
    }

    void push_back(T &&value)
    {
        emplace_back(std::move(value));
    }

    template <typename... Args>
    void emplace_back(Args &&...args)
    {
        if (size_ >= capacity_)
            reallocate(capacity_ == 0 ? 1 : capacity_ * 2);
        new (&data_[size_]) T(std::forward<Args>(args)...);
        size_++;
    }

    void pop_back()
    {
        data_[--size_].~T();
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

    void clear()
    {
        for (size_t i = 0; i < size_; ++i)
            data_[i].~T();
        size_ = 0; // Don't free memory, just reset logical size
    }

    bool empty() const
    {
        return size_ == 0;
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
        if (new_size < size_)
        {
            for (size_t i = new_size; i < size_; ++i)
                data_[i].~T();
        }
        else
        {
            for (size_t i = size_; i < new_size; ++i)
                new (&data_[i]) T();
        }
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
            new_data[i] = std::move(data_[i]);
        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }
};

int main()
{
    MyVector<int> vec;

    std::cout << "Pushing back 0 to 4\n";
    for (int i = 0; i < 5; ++i)
    {
        vec.push_back(i);
        std::cout << "Added " << i << ", size: " << vec.size() << ", capacity: " << vec.capacity() << "\n";
    }

    std::cout << "Contents: ";
    for (size_t i = 0; i < vec.size(); ++i)
        std::cout << vec[i] << " ";
    std::cout << "\n";

    vec.pop_back();
    std::cout << "After pop_back(), size: " << vec.size() << "\n";

    vec.resize(10);
    std::cout << "After resize(10), size: " << vec.size() << ", capacity: " << vec.capacity() << "\n";

    vec.clear();
    std::cout << "After clear(), size: " << vec.size() << ", is empty? " << std::boolalpha << vec.empty() << "\n";

    return 0;
}
