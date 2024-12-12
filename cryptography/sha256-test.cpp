#include <array>
#include <fstream>
#include <iostream>
#include <iomanip>
// #include <list>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <sstream>
#include <string>
#include <vector>

std::string sha256_v3(const unsigned char* buffer, size_t size)
{
    const EVP_MD *message_digest = EVP_get_digestbyname("SHA256");
    if (message_digest == NULL)
        std::cerr << "SHA256 unknown message digest" << std::endl;

    EVP_MD_CTX *md_context = EVP_MD_CTX_new();
    EVP_DigestInit_ex2(md_context, message_digest, NULL);
    
    if (EVP_DigestUpdate(md_context, buffer, size) == 0)
        std::cerr << "SHA256 update failed" << std::endl;
    
    std::array<unsigned char, EVP_MAX_MD_SIZE> digest;
    digest.fill('\0');
    unsigned int digest_length = 0;
    if (EVP_DigestFinal_ex(md_context, digest.data(), &digest_length) == 0)
        std::cerr << "SHA256 final failed" << std::endl;
    EVP_MD_CTX_free(md_context);

    std::ostringstream ss;
    for (unsigned int i = 0; i < digest_length; ++i) {
        ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(digest[i]);
    }
    return ss.str();
}

std::string sha256_v1(const unsigned char* buffer, size_t size)
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
        ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(digest[i]);
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

    std::cout << "SHA256 v1: " << sha256_v1(buffer, buffer_size) << std::endl;
    std::cout << "SHA256 v3: " << sha256_v3(buffer, buffer_size) << std::endl;
    std::free(buffer);

    return 0;
}
