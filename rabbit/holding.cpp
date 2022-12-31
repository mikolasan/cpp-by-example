#include <iostream>
#include <cstdint>

int main(int argc, char const *argv[])
{
    int64_t range = 10;
    int64_t max1 = 0xFFFFFFFF + 1;
    int64_t max2 = 0x100000000;
    int64_t t1 = (max1 - range) % range;
    int64_t t2 = (max2 - range) % range;
    int64_t t3 = (-(uint32_t)range) % range;
    
    std::cout << t1 << std::endl;
    std::cout << t2 << std::endl;
    std::cout << t3 << std::endl;
}






