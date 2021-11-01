#include <iostream>

struct SimpleStruct {
    int bar;
};

void foo(SimpleStruct** s) {
    *s = new SimpleStruct;
    (*s)->bar = 42;
}

int main(int argc, char const *argv[])
{
    SimpleStruct* x = nullptr;
    foo(&x);
    std::cout << x->bar << std::endl;
    delete x;
    return 0;
}
