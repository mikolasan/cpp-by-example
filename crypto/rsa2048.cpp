#include <iostream>
#include <cstring>

#include <openssl/engine.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

// https://crypto.stackexchange.com/questions/79604/private-exponent-on-rsa-key
// https://cryptobook.nakov.com/asymmetric-key-ciphers/the-rsa-cryptosystem-concepts
// ssh-keygen -t rsa -m PEM -b 2048
// https://lapo.it/asn1js

// Function to print error messages
void handleErrors()
{
    ERR_print_errors_fp(stderr);
    throw std::runtime_error("OpenSSL error");
}

// Function to encrypt data using RSA
std::string rsaEncrypt(const std::string& data,
                       const std::string& modulus,
                       const std::string& pubExponent)
{
    EVP_PKEY_CTX *ctx;
    ENGINE *eng;
    unsigned char *out;
    unsigned char *in;
    size_t outlen;
    size_t inlen;
    EVP_PKEY *key;


    eng = 
    /*
    * NB: assumes eng, key, in, inlen are already set up,
    * and that key is an RSA public key
    */
    ctx = EVP_PKEY_CTX_new(key, eng);
    if (!ctx)
        /* Error occurred */
    if (EVP_PKEY_encrypt_init(ctx) <= 0)
        /* Error */
    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0)
        /* Error */

    /* Determine buffer length */
    if (EVP_PKEY_encrypt(ctx, NULL, &outlen, in, inlen) <= 0)
        /* Error */

    out = OPENSSL_malloc(outlen);

    if (!out)
        /* malloc failure */

    if (EVP_PKEY_encrypt(ctx, out, &outlen, in, inlen) <= 0)
        /* Error */

    /* Encrypted data is outlen bytes written to buffer out */

    RSA* rsaKey = nullptr;
    BIO* memBio = nullptr;
    std::string encryptedData;

    try {
        // Create RSA object
        rsaKey = RSA_new();
        if (rsaKey == nullptr)
            handleErrors();

        // Set the RSA modulus and public exponent
        rsaKey->n = BN_new();
        if (BN_hex2bn(&rsaKey->n, modulus.c_str()) == 0)
            handleErrors();

        rsaKey->e = BN_new();
        if (BN_hex2bn(&rsaKey->e, pubExponent.c_str()) == 0)
            handleErrors();

        // Create memory BIO to hold the encrypted data
        memBio = BIO_new(BIO_s_mem());
        if (memBio == nullptr)
            handleErrors();

        // Encrypt the data using RSA public key
        int encryptLen = RSA_public_encrypt(data.size(),
                                            reinterpret_cast<const unsigned char*>(data.c_str()),
                                            reinterpret_cast<unsigned char*>(BIO_ptr(memBio)),
                                            rsaKey,
                                            RSA_PKCS1_PADDING);
        if (encryptLen == -1)
            handleErrors();

        // Read the encrypted data from memory BIO
        char* encryptedDataBuf = nullptr;
        long encryptedDataLen = BIO_get_mem_data(memBio, &encryptedDataBuf);
        encryptedData.assign(encryptedDataBuf, encryptedDataLen);

        // Clean up
        RSA_free(rsaKey);
        BIO_free(memBio);

        return encryptedData;
    } catch (...) {
        if (rsaKey != nullptr)
            RSA_free(rsaKey);
        if (memBio != nullptr)
            BIO_free(memBio);
        throw;
    }
}

int main()
{

    std::string modulus = R"modulus(161231462693501180625913283123600427023425282282838782335335072446988)modulus";
    std::string privateExponent = R"exp(440245526166682322394853642222265169884072886256665729237310796873478)exp";
    std::string publicExponent = "0";
    std::string hashAlgorithm = "sha256";
    std::string encryptionAlgorithm = "rsa2048";

    try {
        // RSA* rsaKey = RSA_new();
        // if (rsaKey == nullptr)
        //     handleErrors();

        // rsaKey->n = BN_new();
        // rsaKey->e = BN_new();
        // rsaKey->d = BN_new();
        // if (rsaKey->n == nullptr || rsaKey->e == nullptr || rsaKey->d == nullptr)
        //     handleErrors();

        // // Set the RSA modulus, private exponent, and public exponent
        // if (BN_hex2bn(&rsaKey->n, modulus.c_str()) == 0 ||
        //     BN_hex2bn(&rsaKey->e, publicExponent.c_str()) == 0 ||
        //     BN_hex2bn(&rsaKey->d, privateExponent.c_str()) == 0)
        // {
        //     handleErrors();
        // }

        // // Create a hash context
        // EVP_MD_CTX* mdCtx = EVP_MD_CTX_new();
        // if (mdCtx == nullptr)
        //     handleErrors();

        // // Initialize the hash context with the chosen hash algorithm
        // const EVP_MD* md = EVP_get_digestbyname(hashAlgorithm.c_str());
        // if (md == nullptr)
        //     handleErrors();
        // if (EVP_DigestInit_ex(mdCtx, md, nullptr) == 0)
        //     handleErrors();

        // // Input text data to be encrypted
        // std::string plaintext = "Hello, world!";

        // // Hash the plaintext
        // if (EVP_DigestUpdate(mdCtx, plaintext.c_str(), plaintext.size()) == 0)
        //     handleErrors();

        // // Finalize the hash and get the digest
        // unsigned char hash[EVP_MAX_MD_SIZE];
        // unsigned int hashLen;
        // if (EVP_DigestFinal_ex(mdCtx, hash, &hashLen) == 0)
        //     handleErrors();

        // // Clean up the hash context
        // EVP_MD_CTX_free(mdCtx);

        // // Encrypt the hashed data
        // std::string encryptedData = rsaEncrypt(std::string(reinterpret_cast<char*>(hash), hashLen),
        //                                        modulus,
        //                                        publicExponent);

        // // Print the encrypted data
        // std::cout << "Encrypted data: " << encryptedData << std::endl;

        // // Clean up
        // RSA_free(rsaKey);
        // EVP_cleanup();
        // ERR_free_strings();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}