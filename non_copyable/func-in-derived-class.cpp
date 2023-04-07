#include <iostream>

class Base {
public:
    virtual ~Base() {} // to make it polymorphic
};

class Derived : public Base {
public:
    Derived(int s) {
        stash = new int(s);
    }
    explicit operator bool() const {
        return *stash == 42;
    }
private:
    int* stash;
};

int main(int argc, char const *argv[])
{
    Base* a = new Derived(43);
    auto d = dynamic_cast<Derived*>(a);
    std::cout << std::boolalpha
        << static_cast<bool>(*d)
        << std::endl;
    delete a;
    return 0;
}
