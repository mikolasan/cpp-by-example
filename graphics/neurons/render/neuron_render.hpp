#pragma once

#include <iostream>
#include <memory>
#include <vector>

#include <bx/math.h>
#include <bgfx/bgfx.h>

#include "render_strategy.h"


struct Neuron;

struct PosColorVertex
{
  float m_x;
  float m_y;
  float m_z;
  uint32_t m_abgr;

  static void init()
  {
    ms_layout
      .begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
      .end();
  };

  static bgfx::VertexLayout ms_layout;
};

struct NeuronVisualContext : VisualContext {
  size_t idx;
  bx::Vec3 position;
  std::shared_ptr<Neuron> neuron;

  explicit NeuronVisualContext(std::shared_ptr<Neuron> neuron) : position({0.0f, 0.0f, 0.0f}), neuron(neuron) {}
};

// TODO: many classes have one instance of subclass 
// that is initialized in the first constructor of this class
struct NeuronRenderStrategy : RenderStrategy {
  std::shared_ptr<NeuronVisualContext> ctx;

  NeuronRenderStrategy(std::shared_ptr<NeuronVisualContext> ctx) :
    ctx(ctx) {
  }

  static void generateSphereMesh(
    std::vector<PosColorVertex>& vertices,
    std::vector<uint16_t>& indices,
    int stacks = 2, // vertical segments (horizontal lines make stack of pancakes)
    int slices = 6, // horizontal segments (vertical lines make slices like with apples or oranges)
    float radius = 0.9f);

  void init() override {
    std::cerr << "call 'init_once' instead" << std::endl;
  }

  static void init_once();

  bx::Vec3 get_position() {
    return ctx->position;
  }

  void update(float dt) override {
    // Optional: nothing for now
  }

  void draw(float time) const override {
    // size_t idx = ctx->idx;
    // const bx::Vec3& pos = ctx->positions[idx];

    // bx::Vec3 c = ctx->neuron.spiked ?
    //   bx::Vec3{ 1, 1, 0 } :
    //   bx::Vec3{ ctx->neuron.v, 0, 1 - ctx->neuron.v };
    // draw_sphere(pos, 0.2f, c);
  }

  void destroy() override {

    bgfx::destroy(m_ibh);
    bgfx::destroy(m_vbh);
  }

  static bgfx::VertexBufferHandle m_vbh;
  static bgfx::IndexBufferHandle  m_ibh;
};