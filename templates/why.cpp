#include <algorithm>
#include <iostream>

template <size_t N>
struct StringLiteral {
    constexpr StringLiteral(char const (&str)[N]) {
        std::copy_n(str, N, value);
    }
    char value[N];
    bool operator==(const StringLiteral& other) {
        std::cout << "== operator !" << std::endl;
        return false;
    }
};

template <StringLiteral name>
static int global = 0;

template <StringLiteral name, typename T>
T local;

// #define _(name) global<#name>

#define let(name: type) local<#name, #type>
#define _(name) local<#name, auto>

int main(int argc, char const *argv[])
{
    // global<"multi word variable"> = 41;
    // _(multi word variable) = 41;
    // _(yet another variabl) = 41;
    // bool test = _(multi word variable) == _(yet another variabl);
    // std::cout << _(multi word variable) << " == " << _(yet another variabl) <<  " = " << std::boolalpha << test << std::endl;
    // local<"test 1", int> = 1;
    // std::cout << local<"test 1", int> << std::endl;

    let(test 1: int) = 1;
    std::cout << _(test 1) << std::endl;
    
    return 0;
}
