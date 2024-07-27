#include <iostream>

void foo(void* bar) {
    // cast at your own risk
    std::cout << *reinterpret_cast<double*>(bar) << std::endl;
}

int main(int argc, char const *argv[])
{
    // old school, cast to void
    int i = 42;
    foo((void*)&i);
    double p = 3.14;
    foo((void*)&p);
    return 0;
}

// Possible output:
// -2.94975e-151
// 3.14
