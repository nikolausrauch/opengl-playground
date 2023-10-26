#include "manager.h"

#include "viewer/core/msg_bus.h"
#include "viewer/glfw/window.h"

#include "viewer/opengl/shaderprogram.h"
#include "viewer/opengl/vertexarray.h"
#include "viewer/opengl/texture.h"

#include <glm/ext/matrix_clip_space.hpp>

namespace imgui
{

static const char* vertex_shader =
        GLSL_CODE(330,
        uniform mat4 proj;

        layout(location = 0) in vec2 position;
        layout(location = 1) in vec2 uv;
        layout(location = 2) in vec4 color;

        out vec2 fragUV;
        out vec4 fragColor;

        void main()
        {
            fragUV = uv;
            fragColor = color;
            gl_Position = proj * vec4(position.xy, 0, 1);
        });

static const char* frag_shader =
        GLSL_CODE(330,
        uniform sampler2D fontTexture;

        in vec2 fragUV;
        in vec4 fragColor;

        out vec4 out_Color;

        void main()
        {
            out_Color = fragColor * texture(fontTexture, fragUV.st).rgba;
        });

manager::manager(core::msg_bus& bus, core::window& window, opengl::context& context)
    : m_msg_bus(bus), m_window(window), m_context(context)
{
    /*--------- events ---------*/
    bus.connect<msg::key>(this);
    bus.connect<msg::key_char>(this);
    bus.connect<msg::mouse_button>(this);
    bus.connect<msg::mouse_scroll>(this);


    m_mouse_buttons = {false, false, false};

    mImguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(mImguiContext);
    mImPlotContext = ImPlot::CreateContext();
    ImPlot::SetCurrentContext(mImPlotContext);

    ImGui::StyleColorsDark();
    auto& style = ImGui::GetStyle();
    style.FrameBorderSize = 1.0f;

    /**** GLFW ImGui Bindings ****/
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendPlatformName = "imgui_impl_glfw_opengl3";
    io.ClipboardUserData = static_cast<glfw::window&>(window).handle();

    io.KeyMap[ImGuiKey_Tab]         = static_cast<int>(core::keyboard::key::tab);
    io.KeyMap[ImGuiKey_LeftArrow]   = static_cast<int>(core::keyboard::key::left);
    io.KeyMap[ImGuiKey_RightArrow]  = static_cast<int>(core::keyboard::key::right);
    io.KeyMap[ImGuiKey_UpArrow]     = static_cast<int>(core::keyboard::key::up);
    io.KeyMap[ImGuiKey_DownArrow]   = static_cast<int>(core::keyboard::key::down);
    io.KeyMap[ImGuiKey_PageUp]      = static_cast<int>(core::keyboard::key::page_up);
    io.KeyMap[ImGuiKey_PageDown]    = static_cast<int>(core::keyboard::key::page_down);
    io.KeyMap[ImGuiKey_Home]        = static_cast<int>(core::keyboard::key::home);
    io.KeyMap[ImGuiKey_End]         = static_cast<int>(core::keyboard::key::end);
    io.KeyMap[ImGuiKey_Insert]      = static_cast<int>(core::keyboard::key::insert);
    io.KeyMap[ImGuiKey_Delete]      = static_cast<int>(core::keyboard::key::del);
    io.KeyMap[ImGuiKey_Backspace]   = static_cast<int>(core::keyboard::key::backspace);
    io.KeyMap[ImGuiKey_Space]       = static_cast<int>(core::keyboard::key::space);
    io.KeyMap[ImGuiKey_Enter]       = static_cast<int>(core::keyboard::key::enter);
    io.KeyMap[ImGuiKey_Escape]      = static_cast<int>(core::keyboard::key::escape);
    io.KeyMap[ImGuiKey_KeyPadEnter] = static_cast<int>(core::keyboard::key::kp_enter);
    io.KeyMap[ImGuiKey_A]           = static_cast<int>(core::keyboard::key::a);
    io.KeyMap[ImGuiKey_C]           = static_cast<int>(core::keyboard::key::c);
    io.KeyMap[ImGuiKey_V]           = static_cast<int>(core::keyboard::key::v);
    io.KeyMap[ImGuiKey_X]           = static_cast<int>(core::keyboard::key::x);
    io.KeyMap[ImGuiKey_Y]           = static_cast<int>(core::keyboard::key::y);
    io.KeyMap[ImGuiKey_Z]           = static_cast<int>(core::keyboard::key::z);

    /* shader */
    m_shader = context.make_shader();
    m_shader->attach(vertex_shader, opengl::shader_type::vertex);
    m_shader->attach(frag_shader, opengl::shader_type::fragment);
    m_shader->link();

    /* buffer */
    m_vertexarray = context.make_vertexarray();
    m_index_buffer = context.make_indexbuffer<ImDrawIdx>(opengl::buffer_usage::stream_draw);
    m_vertex_buffer = context.make_vertexbuffer<ImDrawVert>(opengl::buffer_usage::stream_draw);
    m_vertexarray->attach(m_index_buffer);
    m_vertexarray->attach(m_vertex_buffer);
    m_vertexarray->unbind();

    /* texture */
    m_font_texture = context.make_texture(opengl::texture_internal_type::rgba8, opengl::texture_format::rgba, opengl::texture_type::unsigned_byte_);

    if(true)
    {
        ImFontConfig config;
        config.SizePixels = 26.0f;
        config.OversampleH = config.OversampleV = 2;
        config.PixelSnapH = true;
        io.Fonts->AddFontDefault(&config);
    }

    /* load font texture */
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    /* upload font texture data */
    m_font_texture->data(pixels, width, height);
    io.Fonts->TexID = reinterpret_cast<ImTextureID>(m_font_texture.get());
}

manager::~manager()
{
    /*--------- events ---------*/
    m_msg_bus.disconnect<msg::key>(this);
    m_msg_bus.disconnect<msg::key_char>(this);
    m_msg_bus.disconnect<msg::mouse_button>(this);
    m_msg_bus.disconnect<msg::mouse_scroll>(this);

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->TexID = nullptr;

    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

void manager::new_frame(float dt)
{
    ImGuiIO& io = ImGui::GetIO();
    platform_assert(io.Fonts->IsBuilt(), "ImGui Font have not been built!");

    io.DisplaySize = ImVec2( m_window.size().x, m_window.size().y );
    io.DisplayFramebufferScale = ImVec2( m_window.framebuffer_size().x / io.DisplaySize.x, m_window.framebuffer_size().y / io.DisplaySize.y );
    io.DeltaTime = static_cast<float>(dt);

    /* update mouse */
    auto& mouse = m_window.mouse();
    io.MouseDown[0] = m_mouse_buttons[0] || mouse[core::mouse::button::left]; m_mouse_buttons[0] = false;
    io.MouseDown[1] = m_mouse_buttons[1] || mouse[core::mouse::button::right]; m_mouse_buttons[1] = false;
    io.MouseDown[2] = m_mouse_buttons[2] || mouse[core::mouse::button::middle]; m_mouse_buttons[2] = false;


    if(m_window.focused())
    {
        io.MousePos = ImVec2(static_cast<float>(mouse.position().x), static_cast<float>(mouse.position().y));
    }

    // TODO DEBUG
//    mViewer.mScene.mIgnoreMouse = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
//    mViewer.mScene.mIgnoreKeyboard = io.WantCaptureKeyboard;

    /* process imgui draw commands */
    ImGui::NewFrame();
}

void manager::end_frame()
{
    ImGui::Render();

    auto drawData = ImGui::GetDrawData();

    int fb_width = static_cast<int>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
    int fb_height = static_cast<int>(drawData->DisplaySize.y * drawData->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0) return;

    bool state_blend = m_context.enable(opengl::options::blend);
    bool state_scissor = m_context.enable(opengl::options::scissor_test);
    bool state_cull = m_context.disable(opengl::options::cull_face);
    bool state_depth = m_context.disable(opengl::options::depth_test);
    bool state_stencil = m_context.disable(opengl::options::stencil_test);

    auto state_blend_eq = m_context.set(opengl::blend_equation::add);
    auto state_factors = m_context.set(opengl::blend_func_factor_alpha::src_alpha, opengl::blend_func_factor_alpha::one_minus_src_alpha);
    auto state_mode = m_context.set(opengl::polygon_mode::fill);

    auto scissor = m_context.scissor();

    m_shader->bind();
    m_shader->uniform("fontTexture", 0);
    m_shader->uniform("proj", glm::ortho(drawData->DisplayPos.x, drawData->DisplayPos.x + drawData->DisplaySize.x, drawData->DisplayPos.y + drawData->DisplaySize.y, drawData->DisplayPos.y));

    m_vertexarray->bind();

    ImVec2 clip_off = drawData->DisplayPos;
    ImVec2 clip_scale = drawData->FramebufferScale;

    for (int n = 0; n < drawData->CmdListsCount; n++)
    {
        const ImDrawList* cmdList = drawData->CmdLists[n];

        m_vertex_buffer->data(cmdList->VtxBuffer.Data, static_cast<unsigned int>(cmdList->VtxBuffer.Size));
        m_index_buffer->data(cmdList->IdxBuffer.Data, static_cast<unsigned int>(cmdList->IdxBuffer.Size));

        for (int cmd_i = 0; cmd_i < cmdList->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmdList->CmdBuffer[cmd_i];

            ImVec4 clip_rect;
            clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
            clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
            clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
            clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

            if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
            {
                m_context.scissor(static_cast<int>(clip_rect.x), static_cast<int>(fb_height - clip_rect.w),
                                static_cast<int>(clip_rect.z - clip_rect.x), static_cast<int>(clip_rect.w - clip_rect.y));

                auto* texture = reinterpret_cast<opengl::texture*>(pcmd->TextureId);
                texture->bind(0);

                m_context.draw_elements(opengl::primitives::triangles, static_cast<GLsizei>(pcmd->ElemCount) /  3,
                                      opengl::type::unsigned_int_, static_cast<GLsizei>(pcmd->IdxOffset * sizeof(ImDrawIdx)));
            }
        }
    }

    m_vertexarray->unbind();
    m_font_texture->unbind(0);

    m_context.set(opengl::options::blend, state_blend);
    m_context.set(opengl::options::scissor_test, state_scissor);
    m_context.set(opengl::options::cull_face, state_cull);
    m_context.set(opengl::options::depth_test, state_depth);
    m_context.set(opengl::options::stencil_test, state_stencil);
    m_context.scissor(scissor);

    m_context.set(state_blend_eq);
    m_context.set(state_factors.first, state_factors.second);
    m_context.set(state_mode);
}

void manager::receive(const msg::key& key)
{
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[static_cast<int>(key.key)] = key.pressed;

    io.KeyCtrl = io.KeysDown[static_cast<int>(core::keyboard::key::left_control)] || io.KeysDown[static_cast<int>(core::keyboard::key::right_control)];
    io.KeyShift = io.KeysDown[static_cast<int>(core::keyboard::key::left_shift)] || io.KeysDown[static_cast<int>(core::keyboard::key::right_shift)];
    io.KeyAlt = io.KeysDown[static_cast<int>(core::keyboard::key::left_alt)] || io.KeysDown[static_cast<int>(core::keyboard::key::right_alt)];
    io.KeySuper = io.KeysDown[static_cast<int>(core::keyboard::key::left_super)] || io.KeysDown[static_cast<int>(core::keyboard::key::right_super)];
}

void manager::receive(const msg::key_char& character)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(character.code);
}

void manager::receive(const msg::mouse_button& button)
{
    std::size_t idx = static_cast<std::size_t>(button.button);
    m_mouse_buttons[idx] = button.pressed || m_mouse_buttons[idx];
}

void manager::receive(const msg::mouse_scroll& scroll)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel += static_cast<float>(scroll.yoffset);
}


}
