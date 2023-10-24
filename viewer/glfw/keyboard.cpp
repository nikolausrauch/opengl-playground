#include "keyboard.h"

#include "viewer/core/assert.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace glfw
{

namespace detail
{

static constexpr auto g_glfw_keymap = [](){
    std::array<keyboard::key, GLFW_KEY_LAST + 1> keys;
    std::fill(std::begin(keys), std::end(keys), keyboard::key::unkown);

    keys[GLFW_KEY_SPACE] = keyboard::key::space;
    keys[GLFW_KEY_APOSTROPHE] = keyboard::key::apostrophe;
    keys[GLFW_KEY_COMMA] = keyboard::key::comma;
    keys[GLFW_KEY_MINUS] = keyboard::key::minus;
    keys[GLFW_KEY_PERIOD] = keyboard::key::period;
    keys[GLFW_KEY_0] = keyboard::key::num_0;
    keys[GLFW_KEY_1] = keyboard::key::num_1;
    keys[GLFW_KEY_2] = keyboard::key::num_2;
    keys[GLFW_KEY_3] = keyboard::key::num_3;
    keys[GLFW_KEY_4] = keyboard::key::num_4;
    keys[GLFW_KEY_5] = keyboard::key::num_5;
    keys[GLFW_KEY_6] = keyboard::key::num_6;
    keys[GLFW_KEY_7] = keyboard::key::num_7;
    keys[GLFW_KEY_8] = keyboard::key::num_8;
    keys[GLFW_KEY_9] = keyboard::key::num_9;
    keys[GLFW_KEY_SEMICOLON] = keyboard::key::semicolon;
    keys[GLFW_KEY_EQUAL] = keyboard::key::equal;
    keys[GLFW_KEY_A] = keyboard::key::a;
    keys[GLFW_KEY_B] = keyboard::key::b;
    keys[GLFW_KEY_C] = keyboard::key::c;
    keys[GLFW_KEY_D] = keyboard::key::d;
    keys[GLFW_KEY_E] = keyboard::key::e;
    keys[GLFW_KEY_F] = keyboard::key::f;
    keys[GLFW_KEY_G] = keyboard::key::g;
    keys[GLFW_KEY_H] = keyboard::key::h;
    keys[GLFW_KEY_I] = keyboard::key::i;
    keys[GLFW_KEY_J] = keyboard::key::j;
    keys[GLFW_KEY_K] = keyboard::key::k;
    keys[GLFW_KEY_L] = keyboard::key::l;
    keys[GLFW_KEY_M] = keyboard::key::m;
    keys[GLFW_KEY_N] = keyboard::key::n;
    keys[GLFW_KEY_O] = keyboard::key::o;
    keys[GLFW_KEY_P] = keyboard::key::p;
    keys[GLFW_KEY_Q] = keyboard::key::q;
    keys[GLFW_KEY_R] = keyboard::key::r;
    keys[GLFW_KEY_S] = keyboard::key::s;
    keys[GLFW_KEY_T] = keyboard::key::t;
    keys[GLFW_KEY_U] = keyboard::key::u;
    keys[GLFW_KEY_V] = keyboard::key::v;
    keys[GLFW_KEY_W] = keyboard::key::w;
    keys[GLFW_KEY_X] = keyboard::key::x;
    keys[GLFW_KEY_Y] = keyboard::key::y;
    keys[GLFW_KEY_Z] = keyboard::key::z;
    keys[GLFW_KEY_LEFT_BRACKET] = keyboard::key::left_bracket;
    keys[GLFW_KEY_BACKSLASH] = keyboard::key::backslash;
    keys[GLFW_KEY_RIGHT_BRACKET] = keyboard::key::right_bracket;
    keys[GLFW_KEY_GRAVE_ACCENT] = keyboard::key::grave_accent;
    keys[GLFW_KEY_WORLD_1] = keyboard::key::world_1;
    keys[GLFW_KEY_WORLD_2] = keyboard::key::world_2;

    keys[GLFW_KEY_ESCAPE] = keyboard::key::escape;
    keys[GLFW_KEY_ENTER] = keyboard::key::enter;
    keys[GLFW_KEY_TAB] = keyboard::key::tab;
    keys[GLFW_KEY_BACKSPACE] = keyboard::key::backspace;
    keys[GLFW_KEY_INSERT] = keyboard::key::insert;
    keys[GLFW_KEY_DELETE] = keyboard::key::del;
    keys[GLFW_KEY_RIGHT] = keyboard::key::right;
    keys[GLFW_KEY_LEFT] = keyboard::key::left;
    keys[GLFW_KEY_DOWN] = keyboard::key::down;
    keys[GLFW_KEY_UP] = keyboard::key::up;
    keys[GLFW_KEY_PAGE_UP] = keyboard::key::page_up;
    keys[GLFW_KEY_PAGE_DOWN] = keyboard::key::page_down;
    keys[GLFW_KEY_HOME] = keyboard::key::home;
    keys[GLFW_KEY_END] = keyboard::key::end;
    keys[GLFW_KEY_CAPS_LOCK] = keyboard::key::caps_lock;
    keys[GLFW_KEY_SCROLL_LOCK] = keyboard::key::scroll_lock;
    keys[GLFW_KEY_NUM_LOCK] = keyboard::key::num_lock;
    keys[GLFW_KEY_PRINT_SCREEN] = keyboard::key::print_screen;
    keys[GLFW_KEY_PAUSE] = keyboard::key::pause;
    keys[GLFW_KEY_F1] = keyboard::key::f1;
    keys[GLFW_KEY_F2] = keyboard::key::f2;
    keys[GLFW_KEY_F3] = keyboard::key::f3;
    keys[GLFW_KEY_F4] = keyboard::key::f4;
    keys[GLFW_KEY_F5] = keyboard::key::f5;
    keys[GLFW_KEY_F6] = keyboard::key::f6;
    keys[GLFW_KEY_F7] = keyboard::key::f7;
    keys[GLFW_KEY_F8] = keyboard::key::f8;
    keys[GLFW_KEY_F9] = keyboard::key::f9;
    keys[GLFW_KEY_F10] = keyboard::key::f10;
    keys[GLFW_KEY_F11] = keyboard::key::f11;
    keys[GLFW_KEY_F12] = keyboard::key::f12;
    keys[GLFW_KEY_F13] = keyboard::key::f13;
    keys[GLFW_KEY_F14] = keyboard::key::f14;
    keys[GLFW_KEY_F15] = keyboard::key::f15;
    keys[GLFW_KEY_F16] = keyboard::key::f16;
    keys[GLFW_KEY_F17] = keyboard::key::f17;
    keys[GLFW_KEY_F18] = keyboard::key::f18;
    keys[GLFW_KEY_F19] = keyboard::key::f19;
    keys[GLFW_KEY_F20] = keyboard::key::f20;
    keys[GLFW_KEY_F21] = keyboard::key::f21;
    keys[GLFW_KEY_F22] = keyboard::key::f22;
    keys[GLFW_KEY_F23] = keyboard::key::f23;
    keys[GLFW_KEY_F24] = keyboard::key::f24;
    keys[GLFW_KEY_F25] = keyboard::key::f25;
    keys[GLFW_KEY_KP_0] = keyboard::key::kp_0;
    keys[GLFW_KEY_KP_1] = keyboard::key::kp_1;
    keys[GLFW_KEY_KP_2] = keyboard::key::kp_2;
    keys[GLFW_KEY_KP_3] = keyboard::key::kp_3;
    keys[GLFW_KEY_KP_4] = keyboard::key::kp_4;
    keys[GLFW_KEY_KP_5] = keyboard::key::kp_5;
    keys[GLFW_KEY_KP_6] = keyboard::key::kp_6;
    keys[GLFW_KEY_KP_7] = keyboard::key::kp_7;
    keys[GLFW_KEY_KP_8] = keyboard::key::kp_8;
    keys[GLFW_KEY_KP_9] = keyboard::key::kp_9;
    keys[GLFW_KEY_KP_DECIMAL] = keyboard::key::kp_decimal;
    keys[GLFW_KEY_KP_DIVIDE] = keyboard::key::kp_divide;
    keys[GLFW_KEY_KP_MULTIPLY] = keyboard::key::kp_multiply;
    keys[GLFW_KEY_KP_SUBTRACT] = keyboard::key::kp_subtract;
    keys[GLFW_KEY_KP_ADD] = keyboard::key::kp_add;
    keys[GLFW_KEY_KP_ENTER] = keyboard::key::kp_enter;
    keys[GLFW_KEY_KP_EQUAL] = keyboard::key::kp_equal;
    keys[GLFW_KEY_LEFT_SHIFT] = keyboard::key::left_shift;
    keys[GLFW_KEY_LEFT_CONTROL] = keyboard::key::left_control;
    keys[GLFW_KEY_LEFT_ALT] = keyboard::key::left_alt;
    keys[GLFW_KEY_LEFT_SUPER] = keyboard::key::left_super;
    keys[GLFW_KEY_RIGHT_CONTROL] = keyboard::key::right_control;
    keys[GLFW_KEY_RIGHT_ALT] = keyboard::key::right_alt;
    keys[GLFW_KEY_RIGHT_SUPER] = keyboard::key::right_super;
    keys[GLFW_KEY_MENU] = keyboard::key::menu;

    return keys;
}();

void key_cb(GLFWwindow* handle, int key, int scannCode, int action, int mod)
{
    (void) scannCode;
}

void char_cb(GLFWwindow* handle, unsigned int character)
{

}

}

keyboard::keyboard(GLFWwindow *window)
    : m_handle(window)
{
    std::fill(m_key_state.begin(), m_key_state.end(), false);

    glfwSetKeyCallback(m_handle, detail::key_cb);
    glfwSetCharCallback(m_handle, detail::char_cb);
}

void keyboard::sticky_keys(bool enabled)
{
    glfwSetInputMode(m_handle, GLFW_STICKY_KEYS, enabled ? GLFW_TRUE : GLFW_FALSE);
}

bool keyboard::sticky_keys() const
{
    return glfwGetInputMode(m_handle, GLFW_STICKY_KEYS);
}

bool keyboard::operator [](key key) const
{
    return m_key_state[static_cast<int>(key)];
}

void keyboard::glfw_key(int key, int scann_code, int action, int mod)
{
    m_key_state[static_cast<int>(detail::g_glfw_keymap[key])] = (action != GLFW_RELEASE);
}

}
