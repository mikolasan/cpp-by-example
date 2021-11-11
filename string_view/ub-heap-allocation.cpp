#include <iostream>
#include <string_view>

int main(int argc, char const *argv[])
{
    std::string_view v;
    {
        const int size = 15;
        char* const data = new char[size];
        for (size_t i = 0; i < size; ++i) data[i] = i;
        v = {data, size};
        delete data;
    }
    std::cout << static_cast<int>(v[3]) << " - " << v.size() << std::endl;
    return 0;
}