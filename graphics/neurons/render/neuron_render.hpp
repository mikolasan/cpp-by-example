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
  std::vector<bx::Vec3> positions;
  Neuron& neuron;
};

// TODO: many classes have one instance of subclass 
// that is initialized in the first constructor of this class
struct NeuronRenderStrategy : RenderStrategy {
  std::shared_ptr<NeuronVisualContext> ctx;

  NeuronRenderStrategy(std::shared_ptr<NeuronVisualContext> ctx) :
    ctx(ctx) {
  }

  void generateSphereMesh(
    std::vector<PosColorVertex>& vertices,
    std::vector<uint16_t>& indices,
    int stacks = 3, // vertical segments (horizontal lines make stack of pancakes)
    int slices = 5, // horizontal segments (vertical lines make slices like with apples or oranges)
    float radius = 1.0f);

  void init() override {
    std::vector<PosColorVertex> vertices;
    std::vector<uint16_t> indices;
    generateSphereMesh(vertices, indices);

    std::cout << "vertices" << std::endl;
    for (const auto& v : vertices) {
      std::cout << "(" << v.m_x << ", "
        << v.m_y << ", "
        << v.m_z << ") ";
    }
    std::cout << std::endl;

    std::cout << "indices" << std::endl;
    for (const auto& i : indices) {
      std::cout << i << " ";
    }
    std::cout << std::endl;

    // Create static vertex buffer.
    m_vbh = bgfx::createVertexBuffer(
      bgfx::makeRef(vertices.data(), uint32_t(vertices.size() * sizeof(PosColorVertex)))
      , PosColorVertex::ms_layout
    );

    // Create static index buffer.
    m_ibh = bgfx::createIndexBuffer(
      bgfx::makeRef(indices.data(), uint32_t(indices.size() * sizeof(uint16_t)))
    );

  }

  void update(float dt) override {
    // Optional: nothing for now
  }

  void draw() const override {
    size_t idx = ctx->idx;
    const bx::Vec3& pos = ctx->positions[idx];

    bx::Vec3 c = ctx->neuron.spiked ?
      bx::Vec3{ 1, 1, 0 } :
      bx::Vec3{ ctx->neuron.v, 0, 1 - ctx->neuron.v };
    // draw_sphere(pos, 0.2f, c);
  }

  void cleanup() {
    bgfx::destroy(m_ibh);
    bgfx::destroy(m_vbh);
  }

  static bgfx::VertexBufferHandle m_vbh;
  static bgfx::IndexBufferHandle  m_ibh;
};