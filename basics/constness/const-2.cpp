#include <iostream>

int main(int argc, char const *argv[])
{
    const char f = 'a';
    const char* fo = &f;
    const char** foo = &fo;

    char b = 'b';
    char* ba = &b;
    char** bar = &ba;

    b = 'c';
    std::cout << **bar << std::endl;
    
    // foo = bar; // error: invalid conversion from ‘char**’ to ‘const char**’ [-fpermissive]

    // f = 'b'; // error: assignment of read-only variable ‘f’
    // **foo = 'b'; // error: assignment of read-only location ‘* * foo’
    *foo = nullptr; // ok
    foo = nullptr; // ok
    fo = nullptr; // ok

    return 0;
}
