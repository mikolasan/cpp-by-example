#include <cstdarg>
#include <iostream>

enum class Hint {
    COOL_ONE,
    COOL_TWO,
    COOL_THREE,
};

void cool_function(Hint hint, ...) {
    va_list ap;
    va_start(ap, hint);

    switch (hint) {
    case Hint::COOL_ONE:
    {
        unsigned int x = va_arg(ap, unsigned int);
        std::cout << "COOL ONE: " << x << std::endl;
        break;
    }
    case Hint::COOL_TWO:
    {
        double x = va_arg(ap, double);
        double y = va_arg(ap, double);
        std::cout << "COOL TWO: " << x << ", " << y << std::endl;
        break;
    }
    case Hint::COOL_THREE:
    {
        char* c_string = va_arg(ap, char*);
        size_t size = va_arg(ap, size_t);
        std::cout << "COOL THREE: " << c_string << std::endl;
        break;
    }
    default:
        std::cout << "Impossible!" << std::endl;
        break;
    }
    
    va_end(ap);
}


int main(int argc, char const *argv[])
{
    cool_function(Hint::COOL_ONE, 42);
    cool_function(Hint::COOL_TWO, 1.23, 4.56);
    const char* test_string = "array of data";
    cool_function(Hint::COOL_THREE, test_string, sizeof test_string);
    return 0;
}
