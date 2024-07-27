#include <iostream>

template<typename T>
constexpr T max(T a) {
    return a;
}

template<typename T, typename... Rest>
constexpr T max(T a, Rest... rest) {
    T b = max(rest...);
    return a < b ? b : a;
}

int main(int argc, char const *argv[])
{
    constexpr auto max_value_1 = max(1,2,3,4,9);
    std::cout << max_value_1 << std::endl;
    std::cout << max(9,1,2,3,4) << std::endl;
    std::cout << max(1,9,2,3,4) << std::endl;
    // std::cout << max_element({1,2,3,4,9}) << std::endl;
    // std::cout << max_element({9,1,2,3,4}) << std::endl;
    // std::cout << max_element({1,9,2,3,4}) << std::endl;
    return 0;
}
