#include <iostream>

class Base {
public:
  void thing() { some_thing(); do_thing(); }
private:
  void some_thing() {}
  virtual void do_thing() = 0;
};

class Dummy: public Base {
private:
  void do_thing() {}
};

class Subclass: public Base {
private:
  void do_thing() {
    std::cout << "Subclass call\n";
  }
};

int main(int argc, char const *argv[])
{
  Base* t = new Subclass();
  t->thing();
  return 0;
}
