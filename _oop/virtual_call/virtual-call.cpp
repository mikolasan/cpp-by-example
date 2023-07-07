#include <iostream>

class A {
public:
    void call()
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
    virtual void call() const final
    {
        std::cout << "Bc" << std::endl;
    }
};

class C : public B {
public:
    void call() 
    {
        std::cout << "C" << std::endl;
    }
};

int main(int argc, char const *argv[])
{
    const A* a = new A();
    const A* b = new B();
    A* c = new C();

    a->call();
    b->call();
    c->call();
    
    return 0;
}
