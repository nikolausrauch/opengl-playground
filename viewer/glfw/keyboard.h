#pragma once

#include "viewer/core/keyboard.h"

#include <array>

struct GLFWwindow;

namespace glfw
{

class keyboard final : public core::keyboard
{
private:
    GLFWwindow* m_handle;
    std::array<bool, static_cast<int>(core::keyboard::key::last) + 1> m_key_state;

public:
    keyboard(GLFWwindow* window);
    virtual ~keyboard() = default;

    void sticky_keys(bool enabled) override;
    bool sticky_keys() const override;
    bool operator [](key key) const override;

    void glfw_key(int key, int scann_code, int action, int mod);
};

}
