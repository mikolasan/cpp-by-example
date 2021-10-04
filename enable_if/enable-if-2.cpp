#include <iostream>

template<typename T>
void foo(T bar) {
    std::cout << bar << " " 
        << std::boolalpha << (bar == 1) << std::endl;
}

int main(int argc, char const *argv[])
{
    // template. too broad
    foo(1);
    double t = 1.0/3.0 + 1.0;
    foo((t-1.0)*3.0);
    return 0;
}

// Possible output:
// 1 true
// 1 false