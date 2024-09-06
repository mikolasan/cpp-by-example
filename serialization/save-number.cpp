#include <iostream>
#include <stdint.h>

#define MIN(a, b) ((a < b) ? a : b)

void print_number(int64_t n) {
    for (int i = 0; i < 8; ++i) {
        std::cout << static_cast<int>((n >> (i * 8)) & 0xFF) << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char const *argv[])
{
    int little = 1; // little endian
    double n = -10.; // -9223372036854775808LL; 4294967296;
    // std::cin >> n;
    std::cout << n << std::endl;
    size_t n_bytes = 8;
    uint8_t* data = NULL;
    data = (uint8_t*)calloc(n_bytes, 1);
    union {
        int64_t i64;
        uint8_t bytes[4];
    } v;
    v.i64 = static_cast<int64_t>(n);
    print_number(v.i64);
    // std::cout << v.i64 << std::endl;
    // std::cout << sizeof(v) << std::endl;

    if (little) {
        for(size_t i = 0; i < MIN(sizeof(v), n_bytes); ++i) {
            data[i] = v.bytes[i];
        }
    } else {
        for(size_t i = 0; i < MIN(sizeof(v), n_bytes); ++i) {
            data[n_bytes - i - 1] = v.bytes[i];
        }
    }

    for (int i = 0; i < n_bytes; ++i) {
        std::cout << static_cast<int>(data[i]) << " ";
        // std::cout << static_cast<int>(static_cast<int8_t>(data[i])) << " ";
    }
    std::cout << std::endl;

    int64_t vv = 0;
    size_t data_len = 8;
    if (little) {
        std::cout << "-2 ";
        print_number(vv);
        std::cout << static_cast<int>(static_cast<int8_t>(data[data_len - 1])) << std::endl;
        if (0 > static_cast<int8_t>(data[data_len - 1])) {
            int64_t temp = 1LL;
            std::cout << "a " << temp << std::endl;
            temp = 8 * data_len;
            std::cout << "b " << temp << std::endl;
            temp = static_cast<int64_t>(1) << (8 * data_len);
            std::cout << "c " << temp << std::endl;
            temp = temp - 1;
            std::cout << "d " << temp << std::endl;
            temp = ~temp;
            std::cout << "e " << temp << std::endl;
            
            vv = ~((static_cast<int64_t>(1) << 8 * data_len) - 1);
            std::cout << (vv == temp) << std::endl;
            // vv = ~((1LL << (8 * data_len)) - 1);
            std::cout << "-1 ";
            print_number(vv);
        }
        for(size_t i = 0; i < data_len; ++i) {
            vv |= static_cast<int64_t>(data[i]) << (i * 8);
            std::cout << i << " ";
            print_number(vv);
        }
    } else {
        if (0 > static_cast<int8_t>(data[0])) {
            vv = ~((static_cast<int64_t>(1) << 8 * data_len) - 1);
        }
        for(size_t i = 0; i < data_len; ++i) {
            vv |= static_cast<int64_t>(data[data_len - i - 1]) << (i * 8);
        }
    }
    print_number(vv);
    std::cout << vv << std::endl;

    return 0;
}
