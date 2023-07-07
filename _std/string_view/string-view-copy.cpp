#include <cstring>
#include <iostream>
#include <string_view>

int main(int argc, char const *argv[])
{
    std::string_view v;
    std::string_view v_copy;
    {
        char data[] = {0,1,2,3,4};
        v = {data, sizeof(data)};
        char* const data_copy = new char[v.size()];
        std::memcpy(data_copy, v.data(), v.size());
        v_copy = {data_copy, v.size()};
    }
    char new_data[] = {10,11,12,13,14};
    std::cout << static_cast<int>(v[4]) << " - " << v.size() << std::endl;
    std::cout << static_cast<int>(v_copy[4]) << " - " << v.size() << std::endl;
    delete v_copy.data();
    return 0;
}