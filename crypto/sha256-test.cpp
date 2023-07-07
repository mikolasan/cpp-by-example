#include <array>
#include <fstream>
#include <iostream>
// #include <list>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <sstream>
#include <string>
#include <vector>

std::string sha256(const unsigned char* buffer, size_t size)
{
    SHA256_CTX context;
    if (SHA256_Init(&context) == 0)
        std::cerr << "SHA256 context init failed" << std::endl;

    if (SHA256_Update(&context, buffer, size) == 0)
        std::cerr << "SHA256 update failed" << std::endl;
    
    std::array<unsigned char, SHA256_DIGEST_LENGTH> digest;
    digest.fill('\0');
    if (SHA256_Final(digest.data(), &context) == 0)
        std::cerr << "SHA256 final failed" << std::endl;
    
    std::ostringstream ss;
    for (unsigned int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << static_cast<int>(digest[i]);
    }
    return ss.str();
}

int main(int argc, char const *argv[])
{
    std::string in_file_name("bios.bin");
    std::ifstream file(in_file_name, std::ios::binary);
    if (!file) {
        std::cerr << "input file is not good" << std::endl;
        return 1;
    }
    const size_t buffer_size = 4 * 1024 * 1024;
    unsigned char* buffer = reinterpret_cast<unsigned char*>(std::malloc(buffer_size));
    file.read(reinterpret_cast<char*>(buffer), buffer_size);

    std::cout << "SHA256: " << sha256(buffer, buffer_size) << std::endl;
    std::free(buffer);

    return 0;
}
