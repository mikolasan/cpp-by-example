# Constness

## Arrays and pointers

### Intact values

Values of the array cannot be changed. But array itself can be reassigned

```cpp
{{#include ../../basics/constness/const-1.cpp}}
```

### Intact pointer

This preserves the pointer but not the values

```cpp
{{#include ../../basics/constness/const-2.cpp}}
```

So it must be initialized (assigned) only once. You cannot change size of the array, but content can be modified.

```cpp
{{#include ../../basics/constness/const-3.cpp}}
```

### Read-only

Here's the absolute read-only version

```cpp
{{#include ../../basics/constness/const-4.cpp}}
```


## When function changes the pointer

How to make sure that a pointer you have is not going to change? For example, if the function reads elements from an array and will update the pointer to the position where it finished reading.

```cpp
unsigned char* key = NULL;
size_t size = key_size;
OSSL_DECODER_from_data(dctx, key, &size);
// int OSSL_DECODER_from_data(OSSL_DECODER_CTX *ctx, const unsigned char **pdata, size_t *pdata_len);

```

**Wrong**

```cpp
const unsigned char **data = const_cast<const unsigned char**>(&key); // data pointer will be changed
OSSL_DECODER_from_data(dctx, data, &size);

```

**Correct**

```cpp
const unsigned char *data = const_cast<const unsigned char*>(key);
OSSL_DECODER_from_data(dctx, &data, &size);
```

What is const in `const unsigned char *data` vs `const unsigned char **data`?

