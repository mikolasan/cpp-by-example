# Raw pointers

Having proper destructors and an STD container with raw pointers will not save one from leaks.

> if the elements are pointers, the pointed-to objects are not destroyed. 
>
> _[CPP reference](https://en.cppreference.com/w/cpp/container/vector/%7Evector)_

```cpp
{{#include ../../../basics/raw_pointers/raw-pointer-container.cpp}}
```