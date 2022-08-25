#include <iostream>

class A {
public:
    virtual void call() const
    {
        std::cout << "A" << std::endl;
    }
};

class B : public A {
};

class C : public B {
public:
    void call() const final
    {
        std::cout << "C" << std::endl;
    }
};

int main(int argc, char const *argv[])
{
    const A* a = new C();
    const B* b = new C();
    const C* c = new C();

    a->call();
    b->call();
    c->call();
    
    return 0;
}
