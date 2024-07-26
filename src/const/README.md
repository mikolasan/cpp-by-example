# Constness

## When function call changes the pointer

Wrong

```cpp
unsigned char* key = NULL;
size_t size = key_size;
const unsigned char **data = const_cast<const unsigned char**>(&key); // data pointer will be changed
OSSL_DECODER_from_data(dctx, data, &size);

```

Correct

```cpp
const unsigned char *data = const_cast<const unsigned char*>(key);
OSSL_DECODER_from_data(dctx, &data, &size);
```

What is const in `const unsigned char *data` vs `const unsigned char **data`?

