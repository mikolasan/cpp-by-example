// MachineStates.cpp
#include "MachineStates.h"
#include <stdexcept>

void AbstractState::setState(Machine &machine, AbstractState *state) {
  machine.state.reset(state);
}

void AbstractState::updateStock(Machine &machine, unsigned int quantity) {
  machine.stock = quantity;
}

void AbstractState::damage(Machine &machine) {
  setState(machine, new Broken());
};

void AbstractState::fix(Machine &machine) {
  setState(machine, machine.stock > 0
                        ? static_cast<AbstractState *>(new Normal())
                        : static_cast<AbstractState *>(new SoldOut()));
};

void Normal::sell(Machine &machine, unsigned int quantity) {
  unsigned int currStock = machine.getStock();
  if (currStock < quantity) {
    throw std::runtime_error("Not enough stock");
  }

  updateStock(machine, currStock - quantity);

  if (machine.getStock() == 0) {
    setState(machine, new SoldOut());
  }
}

void Normal::refill(Machine &machine, unsigned int quantity) {
  int currStock = machine.getStock();
  updateStock(machine, currStock + quantity);
}

void Normal::fix(Machine &machine) {
  throw std::runtime_error("If it ain't broke, don't fix it!");
};

void SoldOut::sell(Machine &machine, unsigned int quantity) {
  throw std::runtime_error("Sold out!");
}

void SoldOut::refill(Machine &machine, unsigned int quantity) {
  updateStock(machine, quantity);
  setState(machine, new Normal());
}

void SoldOut::fix(Machine &machine) {
  throw std::runtime_error("If it ain't broke, don't fix it!");
};

void Broken::sell(Machine &machine, unsigned int quantity) {
  throw std::runtime_error("Machine is broken! Fix it before sell");
}

void Broken::refill(Machine &machine, unsigned int quantity) {
  throw std::runtime_error("Machine is broken! Fix it before refill");
}