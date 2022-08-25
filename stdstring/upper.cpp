#include <algorithm>
#include <iostream>

int main(int argc, char const *argv[])
{
    std::string text("abcDEFghi!!!😎???");
    std::string prefix("__");
    auto prefix_size = prefix.size();
    if (prefix.size() < text.size()) {
        prefix.resize(prefix.size() + text.size());
    }
    std::transform(text.begin(), text.end(), prefix.begin() + prefix_size,
        [](unsigned char c) { return std::toupper(c); });
    
    std::cout << text << std::endl;
    std::cout << prefix << std::endl;
    return 0;
}
