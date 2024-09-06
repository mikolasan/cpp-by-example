#include <iostream>

enum class Hint {
    COOL_ONE,
    COOL_TWO,
    COOL_THREE,
};

template<typename... Ts>
void cool_function(Hint hint, Ts... args) {
    switch (hint) {
    case Hint::COOL_ONE:
    {
        // std::cout << "COOL ONE: " << (args...) << std::endl;
        break;
    }
    case Hint::COOL_TWO:
    {
        std::cout << "COOL TWO: ";
        int dummy[sizeof...(Ts)] = {(std::cout << args, 0)...};
        std::cout << std::endl;
        break;
    }
    case Hint::COOL_THREE:
    {
        std::cout << "COOL THREE: ";
        // auto print_cool_three = [](const char* string, size_t size) {
        //     std::cout << string;
        // };
        // print_cool_three(args...);
        std::cout << std::endl;
        break;
    }
    default:
        std::cout << "Impossible!" << std::endl;
        break;
    }
}


int main(int argc, char const *argv[])
{
    cool_function(Hint::COOL_ONE, 42);
    cool_function(Hint::COOL_TWO, 1.23, 4.56);
    const char* test_string = "array of data";
    cool_function(Hint::COOL_THREE, test_string, sizeof test_string);
    return 0;
}
