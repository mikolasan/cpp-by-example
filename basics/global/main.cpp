#include <iostream>

#include "somewhere.h"
#include "other_place.h"

int main(int argc, char const *argv[])
{
    std::cout << "Original value (namespace): " << ggg::global_variable << std::endl;
    std::cout << "Original value (class): " << mmm::global_variable_that_works << std::endl;
    foo();
    std::cout << "Changed value (namespace): " << ggg::global_variable << std::endl;
    std::cout << "Changed value (class): " << mmm::global_variable_that_works << std::endl;
    return 0;
}
