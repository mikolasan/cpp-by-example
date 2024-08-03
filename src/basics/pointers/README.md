# Pointers

## Smart pointers

Elegant code from [this github issue](https://github.com/openssl/openssl/issues/20855). Look how destructor is called in the style of `defer`-ed functions when the object goes out of scope.

```cpp
int main() {
    std::string filename = "test.pem";
    std::string password = "password";
    {
        std::unique_ptr<EVP_PKEY, decltype(&::EVP_PKEY_free)> key(EVP_RSA_gen(2048), ::EVP_PKEY_free);

        std::unique_ptr<BIO, decltype(&::BIO_free)> out(BIO_new(BIO_s_file()), ::BIO_free);
        if (out != nullptr) {
            if (BIO_write_filename(out.get(), const_cast<char *>(filename.c_str())) > 0) {
                int size = PEM_write_bio_PrivateKey(out.get(), key.get(), nullptr, nullptr, 0,
                                                    nullptr, const_cast<char *>(password.data()));
                std::cout << size << std::endl;
            }
        }
    }
    {
        std::string invalidPassword = "invalid";
        std::unique_ptr<BIO, decltype(&::BIO_free)> in(BIO_new(BIO_s_file()), ::BIO_free);
        if (in != nullptr) {
            if (BIO_read_filename(in.get(), filename.c_str()) > 0) {
                EVP_PKEY *pkey = PEM_read_bio_PrivateKey(in.get(), nullptr, nullptr, nullptr);
                std::cout << (pkey == nullptr) << std::endl;
            }
        }
    }
}
```
