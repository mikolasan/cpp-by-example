#include <exception>
#include <iostream>
#include "cxxopts.hpp"

int main(int argc, char const *argv[])
{
  cxxopts::Options options("MyProgram", "One line description of MyProgram");
  options.add_options()
    ("d,debug", "Enable debugging") // a bool parameter
    ("i,integer", "Int param", cxxopts::value<int>())
    ("f,file", "File name", cxxopts::value<std::string>())
    ("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"))
    ;
  try {
    auto result = options.parse(argc, argv);
    if (result.count("i")) {
      std::cout << result["i"].as<int>() << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << "WAT!!! " << e.what() << std::endl;
    return 1;
  }
  std::cout << "I AM the CRYPTOR" << std::endl;
  return 0;
}
