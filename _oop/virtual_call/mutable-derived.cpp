#include <iostream>

class A {
public:
    virtual void call()
    {
        std::cout << "A" << std::endl;
    }

    virtual void call() const
    {
        std::cout << "Ac" << std::endl;
    }
};

class B : public A {
public:
    void call()
    {
        std::cout << "B" << std::endl;
        data++;
    }
private:
    int data;
};

int main(int argc, char const *argv[])
{
    B b;
    b.call();
    A& x = b;
    x.call();

    return 0;
}
