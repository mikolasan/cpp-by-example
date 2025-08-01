#pragma once

// #include <algorithm>
// #include <memory>
// #include <vector>

// #include <bx/math.h>

struct VisualContext {};

struct RenderStrategy {
    virtual void init() = 0;
    virtual void update(float dt) = 0;
    virtual void draw(float time) const = 0;
    virtual void destroy() = 0;
    virtual ~RenderStrategy() = default;
};

