#include "network.hpp"
#include <iostream>

int main() {
    Network net(10);
    for (int t = 0; t < 1000; ++t) {
        net.neurons[0].v += 1.5f;  // inject current to neuron 0
        net.step();

        for (size_t i = 0; i < net.neurons.size(); ++i)
            if (net.neurons[i].spiked)
                std::cout << "t=" << net.time << " neuron " << i << " spiked\n";
    }
}