// MachineStates.h
#pragma once

#include "Machine.h"
#include <iostream>

class Machine;

class AbstractState {
public:
  virtual void sell(Machine &machine, unsigned int quantity) = 0;
  virtual void refill(Machine &machine, unsigned int quantity) = 0;
  virtual void damage(Machine &machine);
  virtual void fix(Machine &machine);

protected:
  ~AbstractState() { std::cout << "Magically called\n"; };
  void setState(Machine &machine, AbstractState *st);
  void updateStock(Machine &machine, unsigned int quantity);
};

struct D // deleter
{
  void operator()(AbstractState* p) const
  {
    std::cout << "D is deleting a Foo\n";
    if (!std::is_same<AbstractState, decltype(*p)>::value
        && std::is_base_of<AbstractState, decltype(*p)>::value){
      delete dynamic_cast<decltype(p)>(p);
    }
  };
};

class Normal : public AbstractState {
public:
  virtual void sell(Machine &machine, unsigned int quantity) override;
  virtual void refill(Machine &machine, unsigned int quantity) override;
  virtual void fix(Machine &machine) override;
  ~Normal() = default;
};

class SoldOut : public AbstractState {
public:
  virtual void sell(Machine &machine, unsigned int quantity) override;
  virtual void refill(Machine &machine, unsigned int quantity) override;
  virtual void fix(Machine &machine) override;
  ~SoldOut() = default;
};

class Broken : public AbstractState {
public:
  virtual void sell(Machine &machine, unsigned int quantity) override;
  virtual void refill(Machine &machine, unsigned int quantity) override;
  ~Broken() = default;
};