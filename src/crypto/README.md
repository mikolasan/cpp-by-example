# RSA

You cannot do a "private encrypt" operation using EVP_PKEY_encrypt. That function only does a public encrypt operation. Similarly you cannot do "public decrypt" with EVP_PKEY_decrypt.

In reality a "private encrypt" operation is more commonly known as a signature. You need to use EVP_PKEY_sign for that. A "public decrypt" operaiton is more commonly known as a verify. You need to use EVP_PKEY_verify for that.