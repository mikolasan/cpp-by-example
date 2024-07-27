# Constness

## Arrays and pointers

Values of the array cannot be changed. But array itself can be assigned

```cpp
{{#include ../../basics/constness/const-1.cpp}}
```

This preserves the pointer but not the values

```cpp
{{#include ../../basics/constness/const-2.cpp}}
```

It must be assigned only once

```cpp
{{#include ../../basics/constness/const-3.cpp}}
```

Here's the absolute read-only version

```cpp
{{#include ../../basics/constness/const-4.cpp}}
```


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

