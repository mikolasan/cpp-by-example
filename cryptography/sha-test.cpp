#include <array>
#include <fstream>
#include <iostream>
#include <list>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <string>
#include <string_view>
#include <vector>

const int READ_BUFFER_SIZE = 1024 * 16;

using digest_array = std::array<unsigned char, 32>;

digest_array sha256(const std::vector<char>& input)
{
    const EVP_MD *md = EVP_get_digestbyname("SHA256");
    if (md == NULL) {
        std::cerr << "sha256 unknown message digest" << std::endl;
    }
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex2(mdctx, md, NULL);
    
    EVP_DigestUpdate(mdctx, input.data(), input.size());
    // std::cerr << "sha256 update failed" << std::endl;
    
    std::array<unsigned char, 32> digest;
    digest.fill('\0');
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len = 0;
    EVP_DigestFinal_ex(mdctx, digest.data(), &md_len);

    // std::cerr << "sha256 final failed" << std::endl;


    // for (auto c : digest) {
    //     std::cout << (unsigned int)c << " ";
    // }
    // std::cout  << std::endl;

    // return std::string_view(reinterpret_cast<char*>(digest.data()), digest.size());
    return digest;
}

// digest_array sha256(const std::string_view& input)
// {
//     const EVP_MD *md = EVP_get_digestbyname("SHA256");
//     if (md == NULL) {
//         std::cerr << "sha256 unknown message digest" << std::endl;
//     }
//     EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
//     EVP_DigestInit_ex2(mdctx, md, NULL);
    
//     EVP_DigestUpdate(mdctx, input.data(), input.size());
//     // std::cerr << "sha256 update failed" << std::endl;
    
//     std::array<unsigned char, 32> digest;
//     digest.fill('\0');
//     unsigned char md_value[EVP_MAX_MD_SIZE];
//     unsigned int md_len = 0;
//     EVP_DigestFinal_ex(mdctx, digest.data(), &md_len);

//     // std::cerr << "sha256 final failed" << std::endl;

//     return digest;

//     // for (auto c : digest) {
//     //     std::cout << std::hex << (unsigned int)c << " ";
        
//     // }
//     // std::cout  << std::endl;

//     // return std::string_view(reinterpret_cast<char*>(digest.data()), digest.size());
// }

// deprecated version
// std::string_view sha256(const std::list<std::string_view>& input)
// {
//     SHA256_CTX context;
//     if (SHA256_Init(&context) == 0) {
//         std::cerr << "sha256 context init failed" << std::endl;
//     }
    
//     for (const auto& i : input) {
//         if (SHA256_Update(&context, i.data(), i.size()) == 0) {
//             std::cerr << "sha256 update failed" << std::endl;
//         }
//     }
    
//     std::array<unsigned char, SHA256_DIGEST_LENGTH> digest;
//     if (SHA256_Final(digest.data(), &context) == 0) {
//         std::cerr << "sha256 final failed" << std::endl;
//     }
//     return std::string_view(digest.begin(), digest.end());
// }

int main(int argc, char const *argv[])
{
    // if file opening fails, then better to do it before any heavy computation
    std::string key_output_file_name("out.key");
    std::ofstream key_file(key_output_file_name, std::ios::binary);
    if (!key_file) {
        std::cerr << "output destination is not good" << std::endl;
        return 1;
    }

    if (argc != 2) {
        std::cerr << "how many input strings?" << std::endl;
        return 1;
    }

    const int n = std::atoi(argv[1]);
    std::list<digest_array> digests;
    for (int i = 0; i < n; ++i) {
        std::string line;
        std::cin >> line;
        // std::cout << line << std::endl;
        std::vector<char> v;
        v.assign(line.begin(), line.end());
        digests.emplace_back(sha256(v));
    }

    digests.emplace_back(sha256({
        'p','8','>','G','h','@','W','q','`','~','M','Z','.','q','>','h','y','_','w','\\'
    }));

    digest_array result;
    result.fill('\0');

    for (int i = 0; i < 32; ++i) {
        // std::cout << (unsigned int)result.at(i);
        for (auto it = digests.begin(); it != digests.end(); ++it) {
            // std::cout << std::hex << (unsigned int)it->at(i) << " ^ ";
            result[i] ^= it->at(i);
        }
        // std::cout << " = " << std::hex <<  (unsigned int)result.at(i) << "\n";
    }

    // for (int i = 0; i < 32; ++i) {
    //     std::cout << std::hex << (unsigned int)result[i] << " ";
    // }
    // std::cout  << std::endl;


    // auto output = sha256(input);
    key_file.write(reinterpret_cast<const char*>(result.data()), result.size());

    return 0;
}
