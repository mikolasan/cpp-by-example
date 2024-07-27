#include <iostream>
#include <memory>

struct SimpleStruct {
    int bar;
};

void foo(std::unique_ptr<SimpleStruct>& s) {
    s = std::make_unique<SimpleStruct>();
    s->bar = 42;
}

int main(int argc, char const *argv[])
{
    std::unique_ptr<SimpleStruct> x;
    foo(x);
    std::cout << x->bar << std::endl;
    return 0;
}
