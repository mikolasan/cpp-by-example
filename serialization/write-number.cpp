#include <cstdint> // uint64_t
#include <cstdlib> // calloc
#include <limits>
#include <string>

static char mem[100];

template<typename T>
void write_number(T v, uint8_t* b, size_t type_size)
{
    for (size_t i = 0; i < type_size; ++i) {
        b[i] = static_cast<uint8_t>(v >> (i * 8));
    }
    b[type_size] = '\0';
}

template<typename T>
void save_pulse_value_into_nvram(const std::string &file_name_prefix, int id, T value) {
    std::string file_name = file_name_prefix + std::to_string(id);
    size_t size = sizeof(T) + 1;
    uint8_t *data = static_cast<uint8_t*>(std::calloc(size, sizeof(uint8_t)));
    write_number<T>(value, data, size);
    free(data);
}


int main(int argc, char const *argv[])
{
    using long_t = uint64_t;
    using int_t = uint32_t;
    using short_t = uint16_t;
    using char_t = uint8_t;

    int id = 1;
    constexpr uint64_t value_long {std::numeric_limits<uint64_t>::max()};
    constexpr uint32_t value_int {std::numeric_limits<uint32_t>::max()};
    constexpr uint16_t value_short {std::numeric_limits<uint16_t>::max()};
    constexpr uint8_t value_char {std::numeric_limits<uint8_t>::max()};

    save_pulse_value_into_nvram<long_t>("test long (uint64)", id, value_long);
    save_pulse_value_into_nvram<unsigned short>("test short (uint16))", id, value_short);
    save_pulse_value_into_nvram<unsigned char>("test char (uint8)", id, value_char);
    return 0;
}
