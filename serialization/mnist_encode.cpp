#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


void convert_mnist_csv_to_binary(const std::string& csv_path, const std::string& bin_path) {
    std::ifstream in(csv_path);
    std::ofstream out(bin_path, std::ios::binary);
    std::string line;

    std::getline(in, line); // Skip header

    while (std::getline(in, line)) {
        std::istringstream ss(line);
        std::string token;
        std::getline(ss, token, ','); // label
        uint8_t label = static_cast<uint8_t>(std::stoi(token));
        out.write(reinterpret_cast<char*>(&label), 1);

        for (int i = 0; i < 784; ++i) {
            std::getline(ss, token, ',');
            uint8_t pixel = static_cast<uint8_t>(std::stoi(token));
            out.write(reinterpret_cast<char*>(&pixel), 1);
        }
    }
}


int main(int argc, char const *argv[])
{
    if (argc != 3) {
        std::cerr << "this program needs a MNIST CSV file and output filenames as its input arguments" << std::endl;
        return 1;
    }

    convert_mnist_csv_to_binary(argv[1], argv[2]);

    return 0;
}