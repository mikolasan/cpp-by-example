# RSA

You cannot do a "private encrypt" operation using `EVP_PKEY_encrypt`. That function only does a public encrypt operation. Similarly you cannot do "public decrypt" with `EVP_PKEY_decrypt`.

In reality a "private encrypt" operation is more commonly known as a signature. You need to use `EVP_PKEY_sign` for that. A "public decrypt" operaiton is more commonly known as a verify. You need to use `EVP_PKEY_verify` for that.

## Load public key

```cpp
OSSL_DECODER_CTX *dctx;
EVP_PKEY *pkey = NULL;
const char *format = "PEM";
const char *structure = NULL; // any structure
const char *keytype = "RSA";
dctx = OSSL_DECODER_CTX_new_for_pkey(&pkey, format, structure,
    keytype,
    OSSL_KEYMGMT_SELECT_PUBLIC_KEY,
    NULL, NULL);

if(!dctx)
{
    unsigned long e = ERR_get_error();
    print_error("Key decoder hiccup: '%X'.\n", e);
    return NULL;
}
size_t size = key_size;
// data pointer will be changed
const unsigned char *data = const_cast<const unsigned char*>(key);
if (!OSSL_DECODER_from_data(dctx, &data, &size)) {
    unsigned long e = ERR_get_error();
    print_error("Key decoding error: '%X'.\n", e);
    print_error("load_rsa RSA error: %s %s %s\n", 
        ERR_lib_error_string(e), 
        ERR_reason_error_string(e),
        ERR_error_string(e, NULL)
    );
}
OSSL_DECODER_CTX_free(dctx);
free(key);
```

## Encypt buffer

```cpp
const size_t pwd_buffer_size = 512;
unsigned char pwd_buffer[pwd_buffer_size];
memset(pwd_buffer, 0x00, pwd_buffer_size);
// signs the `res_k0` using the private key rsa and stores the signature in `pwd_buffer`
size_t pwd_size = pwd_buffer_size;
EVP_PKEY *pkey = rsa;
EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, NULL);
EVP_PKEY_encrypt_init(ctx);
EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING);
EVP_PKEY_encrypt(ctx, pwd_buffer, &pwd_size, res_k0, key_size);
EVP_PKEY_CTX_free(ctx);
```

## Load private key with password

```cpp
OSSL_DECODER_CTX *dctx;
EVP_PKEY *pkey = NULL;
const char *format = "PEM";
const char *structure = NULL; // any structure
const char *keytype = "RSA";
dctx = OSSL_DECODER_CTX_new_for_pkey(&pkey, format, structure,
    keytype,
    OSSL_KEYMGMT_SELECT_KEYPAIR,
    NULL, NULL);

if(!dctx)
{
    int e = ERR_get_error();
    print_error("decoder hiccup: '%X'.\n", e);
    return;
}

OSSL_DECODER_CTX_set_passphrase_cb(dctx, &pass_cb, NULL);

size_t size = key_size;
// data pointer will be changed
const unsigned char *data = const_cast<const unsigned char*>(key); 
if (!OSSL_DECODER_from_data(dctx, &data, &size)) {
    int e = ERR_get_error();
    print_error("decoding error: '%X'.\n", e);
}
OSSL_DECODER_CTX_free(dctx);
```

## Decrypt buffer

```cpp
const size_t buffer_size = 512;
unsigned char crypted_pwd[buffer_size];
memcpy(crypted_pwd, crypted, buffer_size);

unsigned char pwd[buffer_size];
memset(pwd, 0x00, buffer_size);
size_t pwd_size = buffer_size;

EVP_PKEY *pkey = _rsa;
EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, NULL);
EVP_PKEY_decrypt_init(ctx);
EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING);
bool decrypt_ok = EVP_PKEY_decrypt(ctx, pwd, &pwd_size, crypted_pwd, buffer_size) == 1;
EVP_PKEY_CTX_free(ctx);

if(!decrypt_ok)
{
    unsigned long e = ERR_get_error();
    print_error("Can't decrypt '%X'\n", e);
    print_error("decrypt: %s %s %s\n", 
        ERR_lib_error_string(e), 
        ERR_reason_error_string(e),
        ERR_error_string(e, NULL)
    );
    return false;
}
```