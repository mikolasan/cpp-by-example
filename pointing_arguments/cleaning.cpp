#include <iostream>

struct SimpleStruct {
    int bar;
};

void foo(SimpleStruct* s) {
    std::cout << s->bar << std::endl;
    delete s;
    s = nullptr;
}

int main(int argc, char const *argv[])
{
    SimpleStruct* x = new SimpleStruct;
    x->bar = 42;
    foo(x);
    if (x) std::cout << x->bar << std::endl;
    return 0;
}
