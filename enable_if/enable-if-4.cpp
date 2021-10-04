#include <iostream>
#include <type_traits>

template<typename T>
std::enable_if_t<std::is_integral<T>::value>
foo(T bar) {
    std::cout << bar << " " 
        << std::boolalpha << (bar == 1) << std::endl;
}

template<typename T>
std::enable_if_t<std::is_floating_point<T>::value>
foo(T bar) {
    std::cout << "float version " 
        << bar << " " 
        << std::boolalpha << (std::abs(bar - 1.0) < 1e-6) << std::endl;
}

int main(int argc, char const *argv[])
{
    // template and `enable_if`
    foo(1);
    float t1 = 1.f/3.f + 1.f;
    foo((t1-1.f)*3.f);
    double t2 = 1.0/3.0 + 1.0;
    foo((t2-1.0)*3.0);
    return 0;
}

// Possible output:
// 1 true
// float version 1 true
// float version 1 true
