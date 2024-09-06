# Basics

## Global variable

Like really global global variable that is defined in one compilation unit but can be changes in another place. It's one of those cases when "I know what I'm doing"

**somewhere.h**

```cpp
extern int global_variable;
```

**somewhere.cpp**

```cpp
int global_variable = 1;
```

**another_place.cpp**

```cpp
void foo()
{
    global_variable = 2;
}
```

## Heap VS stack