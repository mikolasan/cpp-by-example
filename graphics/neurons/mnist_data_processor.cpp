#include "mnist_data_processor.h"
#include "render/neuron_render.hpp"


const size_t input_size = 784;
const uint32_t width = 28, height = 28;

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

void MnistDataProcessor::init() {
  images = load_mnist_bin("mnist.bin");
  current_image_id = 0;
  cached_texture = BGFX_INVALID_HANDLE;
}

size_t MnistDataProcessor::get_input_size() const {
  return input_size;
}

bx::Vec3 MnistDataProcessor::get_area_size() const {
  return { width, height, 1.0f };
}

size_t MnistDataProcessor::get_max_id() const {
  return images.size() - 1;
}

size_t MnistDataProcessor::get_current_id() const {
  return current_image_id;
}

uint8_t MnistDataProcessor::get_current_label() const {
  return images[current_image_id].label;
}

void MnistDataProcessor::set_current_id(size_t id) {
  if (id != current_image_id) {
    if (bgfx::isValid(cached_texture)) {
      bgfx::destroy(cached_texture);
    }
    cached_texture = create_mnist_texture(images[id].pixels);
  }
  current_image_id = id;
}

bgfx::TextureHandle MnistDataProcessor::create_currnet_texture() {
  if (!bgfx::isValid(cached_texture)) {
    cached_texture = create_mnist_texture(images[current_image_id].pixels);
  }
  return cached_texture;
}

std::vector<uint8_t> MnistDataProcessor::convert_current_to_inputs() {
  std::vector<uint8_t> inputs;
  // TODO: wtf is the type for `pixels` to pass to `begin` and `end` ???
  inputs.assign(
    std::begin(images[current_image_id].pixels),
    std::end(images[current_image_id].pixels));
  return inputs;
}

std::vector<Neuron> MnistDataProcessor::prepare_neurons() const {
  std::vector<Neuron> neurons(input_size);
  bx::Vec3 area_size = get_area_size();

  for (size_t i = 0; i < input_size; ++i) {
    auto ctx = std::make_shared<NeuronVisualContext>(neurons[i]);
    ctx->position = {
      float(i % int32_t(area_size.x)),
      i / area_size.x,
      0.0f };
    neurons[i].render = std::make_shared<NeuronRenderStrategy>(ctx);
  }
}