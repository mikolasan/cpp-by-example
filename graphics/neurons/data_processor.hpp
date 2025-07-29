#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <bx/uint32_t.h>
#include <bgfx/bgfx.h>

const size_t input_size = 784;

struct MnistImage {
  uint8_t label;
  uint8_t pixels[input_size];
};

std::vector<MnistImage> load_mnist_bin(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  std::vector<MnistImage> images;
  MnistImage img;

  while (in.read(reinterpret_cast<char*>(&img), sizeof(MnistImage))) {
    images.push_back(img);
  }
  return images;
}

bgfx::TextureHandle create_mnist_texture(const uint8_t* pixels) {
  const uint32_t width = 28, height = 28;
  std::vector<uint8_t> rgba(width * height * 4);

  for (int i = 0; i < input_size; ++i) {
    uint8_t v = pixels[i];
    rgba[i * 4 + 0] = v;
    rgba[i * 4 + 1] = v;
    rgba[i * 4 + 2] = v;
    rgba[i * 4 + 3] = 255;
  }

  const bgfx::Memory* mem = bgfx::copy(rgba.data(), rgba.size());
  return bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::RGBA8, 0, mem);
}

class DataProcessor {};

class MnistDataProcessor : public DataProcessor {
private:
  std::vector<MnistImage> images;
  size_t current_image_id;
  bgfx::TextureHandle cached_texture;
public:

  void init() {
    images = load_mnist_bin("mnist.bin");
    current_image_id = 0;
    cached_texture = BGFX_INVALID_HANDLE;
  }

  size_t get_input_size() const {
    return input_size;
  }

  size_t get_max_id() const {
    return images.size() - 1;
  }

  size_t get_current_id() const {
    return current_image_id;
  }

  uint8_t get_current_label() const {
    return images[current_image_id].label;
  }

  void set_current_id(size_t id) {
    if (id != current_image_id) {
      if (bgfx::isValid(cached_texture)) {
        bgfx::destroy(cached_texture);
      }
      cached_texture = create_mnist_texture(images[id].pixels);
    }
    current_image_id = id;
  }

  bgfx::TextureHandle create_currnet_texture() {
    if (!bgfx::isValid(cached_texture)) {
      cached_texture = create_mnist_texture(images[current_image_id].pixels);
    }
    return cached_texture;
  }

  std::vector<uint8_t> convert_current_to_inputs() {
    std::vector<uint8_t> inputs;
    // TODO: wtf is the type for `pixels` to pass to `begin` and `end` ???
    inputs.assign(
      std::begin(images[current_image_id].pixels),
      std::end(images[current_image_id].pixels));
    return inputs;
  }

};

#endif