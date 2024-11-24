# 5 things to not bother in C++

## Pseudo destructors

```cpp
typedef int I;
int main() {
  I x = 10;
  x.I::~I();
  x = 20;
}
```

  [Built-in member access operators(https://en.cppreference.com/w/cpp/language/operator_member_access#Built-in_member_access_operators)

## ODR use

ODR use _something_ [so question](https://stackoverflow.com/questions/19630570/what-does-it-mean-to-odr-use-something)

```cpp
struct F {
   static const int g_x = 2;
};

int g_x_plus_1 = F::g_x + 1; // in this context, only the value of g_x is needed.
                             // so it's OK without the definition of g_x

vector<int>  vi;
vi.push_back( F::g_x );      // Error, this is odr-used, push_back(const int & t) expect
                             // a const lvalue, so it's definition must be present
```