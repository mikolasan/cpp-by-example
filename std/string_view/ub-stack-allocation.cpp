#include <iostream>
#include <string_view>

int main(int argc, char const *argv[])
{
    std::string_view v;
    {
        char data[] = {0,1,2,3,4};
        v = {data, sizeof(data)};
    }
    std::cout << static_cast<int>(v[4]) << " - " << v.size() << std::endl;
    return 0;
}
