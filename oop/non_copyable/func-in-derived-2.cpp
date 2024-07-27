#include <iostream>

class Base {
public:
  virtual ~Base() {}
};

class Derived : public Base {
public:
  Derived() = delete;
  Derived(Derived& other) {
    stash = new int(other.unstash());
  }
  Derived(int s) {
    stash = new int(s);
  }
  ~Derived() {
    std::cout << "we delete it right" << std::endl;
    delete stash;
  }
  int unstash() const {
    return *stash;
  }
  explicit operator bool() const {
    return *stash == 42;
  }
private:
  int* stash;
};

int main(int argc, char const *argv[])
{
  Base* a = new Derived(43);
  auto d = dynamic_cast<Derived*>(a);
  std::cout << d->unstash() << std::endl;
  std::cout << std::boolalpha << d->operator bool() << std::endl;
  std::cout << std::boolalpha << static_cast<bool>(d) << std::endl; // this is wrong if you are curious
  Derived dd = *d;
  if (!dd) {
    std::cout << "almost certainly false" << std::endl;
  }
  if (!(*d)) {
    std::cout << "probably also false" << std::endl;
  }
  delete a;
  return 0;
}
