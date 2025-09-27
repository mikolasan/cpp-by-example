#ifndef T_PHI_VERTEX
#define T_PHI_VERTEX

#include <stdint.h>
#include <bgfx/bgfx.h>


struct TPhiVertex
{
  float t;   // longitudinal param
  float phi; // angle around tube
  
  // float mmm; // just an extra to have vec3 
  // (I don't know if POSITION can be vec2 or if vec2 should be texcoord :shrug: )
  // because varying def is sooo mysterious https://github.com/bkaradzic/bgfx/issues/306

  static void init()
  {
    kTubeLayout.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float) // (t, phi, mystery) // TODO: reduce to 2
      .end();
  };

  static bgfx::VertexLayout kTubeLayout;
};

#endif