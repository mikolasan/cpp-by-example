
#include <vector>
#include "bgfx_utils.h" // load_program
#include "debugdraw/debugdraw.h"
#include "pos_color_vertex.h"

struct Ground {

  bgfx::ViewId viewId;
  bgfx::VertexBufferHandle m_vbhGround;
  bgfx::IndexBufferHandle m_ibhGround;
  bgfx::VertexBufferHandle m_vbhGrid;
  bgfx::IndexBufferHandle m_ibhGrid;

  bgfx::UniformHandle u_color;

  bgfx::ProgramHandle m_program;

  Ground(bgfx::ViewId id) : viewId(id) {
    make_ground();
    make_grid();

    m_program = loadProgram("vs_selection", "fs_selection");
  }

  void make_ground() {
    static PosColorVertex s_groundVertices[4] = {
        { -1.0f, 1.0f, 0.0f, 0xff696969 },
        {  1.0f, 1.0f, 0.0f, 0xff696969 },
        { -1.0f, -1.0f,  0.0f, 0xff696969 },
        {  1.0f, -1.0f,  0.0f, 0xff696969 },
    };

    static const uint16_t s_groundIndices[] = {
        0, 1, 2,
        1, 3, 2,
    };

    m_vbhGround = bgfx::createVertexBuffer(
      bgfx::makeRef(s_groundVertices, sizeof(s_groundVertices)),
      PosColorVertex::ms_layout
    );

    m_ibhGround = bgfx::createIndexBuffer(
      bgfx::makeRef(s_groundIndices, sizeof(s_groundIndices))
    );

    u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
  }

  void make_grid() {
    // grid

    std::vector<PosColorVertex> gridVerts;
    std::vector<uint16_t> gridIndices;

    const int gridSize = 20;
    const float spacing = 1.0f;

    uint16_t idx = 0;
    for (int i = -gridSize; i <= gridSize; ++i) {
      float x = i * spacing;
      float zmin = -gridSize * spacing;
      float zmax = gridSize * spacing;

      uint32_t color = (i == 0 ? 0xffffffff : 0xff404040);

      gridVerts.push_back({ x, 0.01f, zmin, color });
      gridVerts.push_back({ x, 0.01f, zmax, color });
      gridIndices.push_back(idx++);
      gridIndices.push_back(idx++);

      float z = i * spacing;
      float xmin = -gridSize * spacing;
      float xmax = gridSize * spacing;

      color = (i == 0 ? 0xffffffff : 0xff404040);

      gridVerts.push_back({ xmin, 0.01f, z, color });
      gridVerts.push_back({ xmax, 0.01f, z, color });
      gridIndices.push_back(idx++);
      gridIndices.push_back(idx++);
    }

    m_vbhGrid = bgfx::createVertexBuffer(
      bgfx::makeRef(gridVerts.data(), sizeof(PosColorVertex) * gridVerts.size()),
      PosColorVertex::ms_layout
    );

    m_ibhGrid = bgfx::createIndexBuffer(
      bgfx::makeRef(gridIndices.data(), sizeof(uint16_t) * gridIndices.size())
    );
  }

  void draw() {
    draw_ground();
    // draw_grid();
    
    DebugDrawEncoder dde;
    dde.begin(viewId); // viewId

    dde.drawGrid(Axis::Y, { 0.0f, 0.0f, 0.0f });
    // const int gridSize = 20;
    // const float spacing = 1.0f;

    // for (int i = -gridSize; i <= gridSize; ++i) {
    //     // Lines parallel to Z
    //     dde.moveTo((float)i * spacing, 0.0f, (float)-gridSize * spacing);
    //     dde.lineTo((float)i * spacing, 0.0f, (float) gridSize * spacing);

    //     // Lines parallel to X
    //     dde.moveTo((float)-gridSize * spacing, 0.0f, (float)i * spacing);
    //     dde.lineTo((float) gridSize * spacing, 0.0f, (float)i * spacing);
    // }

    // // optional axes
    // dde.drawAxis(0.0f, 0.0f, 0.0f);

    dde.end();
  }

  void draw_ground() {
    float selMtx[16];
    bx::mtxIdentity(selMtx);
    bx::mtxSRT(selMtx,
           10.0f, 10.0f, 1.0f,   // scale
           0.0f, 0.0f, 0.0f,   // rotation
          0.0f,0.0f, -3.0f);// translation
    // bx::mtxTranslate(selMtx, -12.0f, -12.0f, 0.0f);
    bgfx::setTransform(selMtx);

    float m_color[4];
    
    m_color[0] = 0.32f;
    m_color[1] = 0.32f;
    m_color[2] = 0.32f;
    m_color[3] = 1.0f;
    bgfx::setUniform(u_color, m_color);

    bgfx::setVertexBuffer(0, m_vbhGround);
    bgfx::setIndexBuffer(m_ibhGround);
    bgfx::setState(BGFX_STATE_WRITE_RGB); // disable culling
    bgfx::submit(viewId, m_program);
  }

  void draw_grid() {
    bgfx::setVertexBuffer(0, m_vbhGrid);
    bgfx::setIndexBuffer(m_ibhGrid);
    bgfx::setState(BGFX_STATE_DEFAULT);
    bgfx::submit(viewId, m_program);
  }

};