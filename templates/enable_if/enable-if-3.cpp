#include <iostream>

template<typename T>
void foo(T bar) {
    std::cout << bar << " " 
        << std::boolalpha << (bar == 1) << std::endl;
}

void foo(double bar) {
    std::cout << "double version " 
        << bar << " " 
        << std::boolalpha << (std::abs(bar - 1.0) < 1e-10) << std::endl;
}

int main(int argc, char const *argv[])
{
    // template and overload. too narrow
    foo(1);
    foo(double(1.0));
    float t = 1.f/3.f + 1.f;
    foo((t-1.f)*3.f);
    return 0;
}

// Possible output:
// 1 true
// double version 1 true
// 1 false
