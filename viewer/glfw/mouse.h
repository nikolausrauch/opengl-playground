#pragma once

#include "viewer/core/mouse.h"

#include <array>
#include <functional>

struct GLFWwindow;

namespace glfw
{

class mouse final : public core::mouse
{
private:
    GLFWwindow* m_handle;
    std::array<bool, static_cast<int>(button::last) + 1>  m_button_state;

public:
    mouse(GLFWwindow* handle);
    ~mouse() = default;

    mouse(const mouse&) = delete;
    mouse& operator = (const mouse&) = delete;

    void cursor_state(core::mouse::cursor state) override;
    glm::vec2 position() const override;
    bool operator[](button button) const override;

    void glfw_button(int button, int action, int mods);
};

}
