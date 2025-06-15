#include <iostream>
#include <unordered_map>
#include <memory>
#include <functional>

class Shape
{
public:
    virtual void draw() = 0;
    virtual ~Shape() = default;
};

// Factory Registry
class ShapeFactory
{
public:
    using Creator = std::function<std::unique_ptr<Shape>()>;

    static ShapeFactory &instance()
    {
        static ShapeFactory factory;
        return factory;
    }

    void registerShape(const std::string &name, Creator creator)
    {
        creators_[name] = std::move(creator);
    }

    std::unique_ptr<Shape> createShape(const std::string &name)
    {
        auto it = creators_.find(name);
        if (it != creators_.end())
            return (it->second)();
        return nullptr;
    }

private:
    std::unordered_map<std::string, Creator> creators_;
};

class Circle : public Shape
{
public:
    void draw() override { std::cout << "Drawing Circle\n"; }

private:
    struct Register
    {
        Register()
        {
            ShapeFactory::instance().registerShape("circle", []
                                                   { return std::make_unique<Circle>(); });
        }
    };
    static Register reg_;
};

Circle::Register Circle::reg_;

class Square : public Shape
{
public:
    void draw() override { std::cout << "Drawing Square\n"; }

private:
    struct Register
    {
        Register()
        {
            ShapeFactory::instance().registerShape("square", []
                                                   { return std::make_unique<Square>(); });
        }
    };
    static Register reg_;
};

Square::Register Square::reg_;

int main()
{
    auto s1 = ShapeFactory::instance().createShape("circle");
    auto s2 = ShapeFactory::instance().createShape("square");

    s1->draw();
    s2->draw();
}