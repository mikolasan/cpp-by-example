#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <ryml_std.hpp>
#include <ryml.hpp>

std::string get_file_contents(const char *filename)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (!in) {
        std::cerr << "could not open " << filename << std::endl;
        exit(1);
    }
    std::ostringstream contents;
    contents << in.rdbuf();
    return contents.str();
}

int main(int argc, char const *argv[]) 
{
    std::string contents = get_file_contents("config.yaml");
    ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(contents));
    ryml::NodeRef foo = tree["foo"];
    for (ryml::NodeRef const& child : foo.children()) {
        std::cout << "key: " << child.key() << " val: " << child.val() << std::endl;
    }

    ryml::NodeRef array = tree["matrix"]["array"];
    for (ryml::NodeRef const& child : array.children()) {
        std::string val;
        child >> val;
        std::cout << "float val: " << std::setprecision (18) << val << std::endl;
    }

    return 0;
}