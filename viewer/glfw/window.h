#pragma once

#include "viewer/core/window.h"

#include <glm/vec2.hpp>

#include <string>


struct GLFWwindow;
namespace core { class msg_bus; }

namespace glfw
{

class mouse;
class keyboard;


class window final : public core::window
{
private:
    GLFWwindow* m_handle;
    core::msg_bus& m_msg_bus;
    std::unique_ptr<glfw::mouse> m_mouse;
    std::unique_ptr<glfw::keyboard> m_keyboard;

    glm::ivec2 m_size_backup;
    glm::ivec2 m_pos_backup;

public:
    window(core::msg_bus& bus, const std::string& title, unsigned int width, unsigned int height);
    virtual ~window();
    window(const window&) = delete;
    window& operator=(const window&) = delete;

    GLFWwindow* handle() const;
    core::msg_bus& bus();

    class core::mouse& mouse() override;
    const class core::mouse& mouse() const override;
    class core::keyboard& keyboard() override;
    const class core::keyboard& keyboard() const override;

    void title(const std::string& title);
    void position(const glm::ivec2& point);
    void size(const glm::ivec2& size);
    void make_active_context();
    void display();

    void vsync(bool enable) override;
    void fullscreen(bool enable) override;

    glm::ivec2 position() const;
    glm::uvec2 size() const override;
    glm::uvec2 framebuffer_size() const override;

    void close() override;
    void show() override;
    void iconify() override;

    bool focused() const override;
    bool iconified() const override;
    bool visible()  const override;
    bool closed()  const override;

    void icon(const asset::handle<asset::image>& img) override;
};

}
