#pragma once

#include "viewer/asset/fwd.h"

#include <glm/fwd.hpp>

namespace core
{

class window
{
public:
    window() = default;
    virtual ~window() = default;
    window(const window&) = delete;
    window& operator=(const window&) = delete;

    virtual glm::uvec2 size() const = 0;
    virtual glm::uvec2 framebuffer_size() const = 0;
    virtual void vsync(bool enable) = 0;
    virtual void fullscreen(bool enable) = 0;

    virtual void close() = 0;
    virtual void show() = 0;
    virtual void iconify() = 0;

    virtual bool focused() const = 0;
    virtual bool iconified() const = 0;
    virtual bool visible()  const = 0;
    virtual bool closed() const = 0;

    virtual void icon(const asset::handle<asset::image>& img) = 0;
};

}
