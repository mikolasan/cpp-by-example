#include <iostream>
#include <string_view>

std::string_view copy(std::string_view src) {
    using Char = std::string_view::value_type;
    Char* const dest = new Char[src.size()];
    src.copy(dest, src.size());
    return {dest, src.size()};
}

int main(int argc, char const *argv[])
{
    std::string_view v, v_copy;
    {
        char data[] = {0,1,2,3,4};
        v = {data, sizeof(data)};
        v_copy = copy(v);
    }
    std::cout << static_cast<int>(v_copy[4]) << " - " << v.size() << std::endl;
    delete v_copy.data();
    return 0;
}