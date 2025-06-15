#include <cstdio>
#include <stdexcept>
#include <string>
#include <iostream>

class FileRAII
{
public:
    FileRAII(const char *filename, const char *mode)
    {
        f_ = fopen(filename, mode);
        if (!f_)
        {
            throw std::runtime_error(std::string("Failed to open file: ") + filename);
        }
    };

    ~FileRAII()
    {
        // this if check is necessary since f_ may have been moved and
        // calling destructor post move/close may cause double close attempt
        // and is not defined behaviour
        if (f_)
        {
            fclose(f_);
        }
    };

    FileRAII(const FileRAII &) = delete;
    FileRAII &operator=(const FileRAII &) = delete;

    FileRAII(FileRAII &&other) noexcept : f_(other.f_)
    {
        other.f_ = nullptr;
    };

    FileRAII &operator=(FileRAII &&other) noexcept
    {
        if (this != &other)
        {
            if (f_)
                fclose(f_);
            f_ = other.f_;
            other.f_ = nullptr;
        }
        return *this;
    };

    FILE *get() const
    {
        return f_;
    };

private:
    FILE *f_;
};

int main()
{
    try
    {
        FileRAII file1("example.txt", "r");
        std::cout << "File opened successfully.\n";

        FileRAII file2 = std::move(file1);
        if (!file1.get())
        {
            std::cout << "file1 has been moved.\n";
        }

        if (file2.get())
        {
            std::cout << "file2 owns the file now.\n";
        }

        char buffer[256];
        if (fgets(buffer, sizeof(buffer), file2.get()))
        {
            std::cout << "First line: " << buffer;
        }
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Exception caught: " << e.what() << '\n';
    }

    return 0;
}