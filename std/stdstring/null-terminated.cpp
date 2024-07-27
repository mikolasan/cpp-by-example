#include <cstring>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char const *argv[])
{
    unsigned char serial_number[] = "12345";
    std::vector<unsigned char> serial_number_2 = {'B','I','O','S'};
	std::string printable = std::string((char*)serial_number);
    std::cout << printable << std::endl;
    std::cout << printable.size() << std::endl;
    const char* a = printable.data();
    std::cout << strlen(a) << std::endl;
    std::cout << std::boolalpha << (a[printable.size()] == '\0') << std::endl; // since c++11
    return 0;
}
