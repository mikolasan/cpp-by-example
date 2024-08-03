# Pointing arguments

What if you have a pointer defined in one place and you want to assign some value in a special function?
How to pass that pointer to the function and receive and 
```cpp
{{#include ../../../basics/pointing_arguments/pointing.cpp}}
```

## C style

In C one would use a pointer for that

```cpp
{{#include ../../../basics/pointing_arguments/c-style.cpp}}
```

## By reference

But in C++ it is logical to use a reference, innit?

```cpp
{{#include ../../../basics/pointing_arguments/by-reference.cpp}}
```

## C++ style

But the clean way would be to use smart pointers

```cpp
{{#include ../../../basics/pointing_arguments/cpp-style.cpp}}
```
