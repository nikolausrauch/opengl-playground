#pragma once

#include "viewer/core/msg.h"
#include "viewer/opengl/indexbuffer.h"
#include "viewer/opengl/vertexbuffer.h"

#include <imgui/imgui.h>
#include <imgui/implot.h>

namespace core { class msg_bus; class window; }
namespace opengl { class context; }

template<> struct opengl::layout<ImDrawVert>
{
    static constexpr attr_info value[] =
    {
        {opengl::type::float_,          2, opengl::buffer_mapping::cast,        offsetof(ImDrawVert, pos)},
        {opengl::type::float_,          2, opengl::buffer_mapping::cast,        offsetof(ImDrawVert, uv)},
        {opengl::type::unsigned_byte_,  4, opengl::buffer_mapping::normalized,  offsetof(ImDrawVert, col)}
    };
};

namespace imgui
{

class manager final
{
private:
    core::msg_bus& m_msg_bus;
    core::window& m_window;
    opengl::context& m_context;
    ImGuiContext* mImguiContext{nullptr};
    ImPlotContext* mImPlotContext{nullptr};
    std::array<bool, 3> m_mouse_buttons{false, false, false};

    opengl::handle<opengl::shader_program> m_shader{nullptr};
    opengl::handle<opengl::texture> m_font_texture{nullptr};
    opengl::handle<opengl::vertexarray> m_vertexarray{nullptr};
    opengl::handle<opengl::indexbuffer<ImDrawIdx>> m_index_buffer{nullptr};
    opengl::handle<opengl::vertexbuffer<ImDrawVert>> m_vertex_buffer{nullptr};

public:
    manager(core::msg_bus& bus, core::window& window, opengl::context& context);
    ~manager();

    void new_frame(float dt);
    void end_frame();

    void receive(const msg::key&);
    void receive(const msg::key_char&);
    void receive(const msg::mouse_button&);
    void receive(const msg::mouse_scroll&);
};

}

