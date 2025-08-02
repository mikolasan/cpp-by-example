#ifndef MNIST_DATA_PROCESSOR_H
#define MNIST_DATA_PROCESSOR_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <bx/math.h>
#include <bx/uint32_t.h>
#include <bgfx/bgfx.h>

#include "data_processor.h"
#include "neuron.hpp"
#include "network.hpp"

struct MnistImage;

class MnistDataProcessor : public DataProcessor {
private:
  std::vector<MnistImage> images;
  size_t current_image_id;
  bgfx::TextureHandle cached_texture;

public:

  void init();

  size_t get_input_size() const;
  bx::Vec3 get_area_size() const override;
  size_t get_max_id() const;
  size_t get_current_id() const;
  uint8_t get_current_label() const;
  void set_current_id(size_t id);
  bgfx::TextureHandle create_currnet_texture();
  std::vector<uint8_t> convert_current_to_inputs();

  Network::NeuronLayer prepare_neurons() const;

};

#endif