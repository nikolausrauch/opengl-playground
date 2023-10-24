#include "module.h"

#include "gamepad.h"
#include "keyboard.h"
#include "mouse.h"
#include "window.h"

#include "engine/core/assert.h"
#include "engine/core/log.h"
#include "engine/debug/profile_sample.h"
#include "engine/message/bus.h"
#include "engine/message/engine.h"

#include <entt/entity/registry.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace glfw
{

namespace detail
{

std::string glfw_error_string(int code)
{
    switch (code)
    {
    case GLFW_NOT_INITIALIZED:		return "GLFW has not been initialized";
    case GLFW_NO_CURRENT_CONTEXT:	return "No context is current for this thread";
    case GLFW_INVALID_ENUM:			return "One of the arguments to the function was an invalid enum value";
    case GLFW_INVALID_VALUE:		return "One of the arguments to the function was an invalid value";
    case GLFW_OUT_OF_MEMORY:		return "A memory allocation failed";
    case GLFW_API_UNAVAILABLE:		return "GLFW could not find support for the requested client API on the system";
    case GLFW_VERSION_UNAVAILABLE:	return "The requested OpenGL or OpenGL ES version is not available";
    case GLFW_PLATFORM_ERROR:		return "A platform - specific error occurred that does not match any of the more specific categories";
    case GLFW_FORMAT_UNAVAILABLE:	return "The requested format is not supported or available";
    default:
        return std::string("Unknown error code: ") + std::to_string(code);
    }
}

void glfw_error_callback(int error, const char* description)
{
    engine_log(core::log::level::error, "GLFW error ({0}) : {1}", error, description);
}

}

void module::startup(entt::registry& registry)
{
    glfwSetErrorCallback(&detail::glfw_error_callback);
    if(!glfwInit())
    {
        engine_assert(false, "Failed to initialize GLFW");
        throw std::runtime_error("failed to initialize GLFW");
    }

    engine_log(core::log::level::info,
               "\twindow.size = {0}x{1}\n\twindow.title = {2}",
               m_config.window_width,
               m_config.window_height,
               m_config.window_title);

    m_window = std::make_unique<window>(m_config.window_title, m_config.window_width, m_config.window_height);
    m_keyboard = std::make_unique<keyboard>(m_window->handle());
    m_mouse = std::make_unique<mouse>(m_window->handle());
    m_gamepads = std::make_unique<gamepad_manager>(m_window->handle(), registry);

    glfwSetWindowUserPointer(m_window->handle(), &registry);
    m_window->vsync(m_config.vsync);

    registry.ctx().emplace<render::window&>(*m_window);
    registry.ctx().emplace<input::keyboard&>(*m_keyboard);
    registry.ctx().emplace<input::mouse&>(*m_mouse);

    auto& gamepads = m_gamepads->gamepads();
    for(auto& gp : gamepads)
    {
        registry.ctx().emplace_as<input::gamepad&>(gp.id(), gp);
    }

    auto& bus = registry.ctx().get<message::bus&>();
    bus.connect<message::frame_start>(this);
}

void module::shutdown(entt::registry& registry)
{
    auto& bus = registry.ctx().get<message::bus&>();
    bus.disconnect<message::frame_start>(this);

    registry.ctx().erase<render::window&>();
    registry.ctx().erase<input::keyboard&>();
    registry.ctx().erase<input::mouse&>();

    auto& gamepads = m_gamepads->gamepads();
    for(auto& gp : gamepads)
    {
        registry.ctx().erase<input::gamepad&>(gp.id());
    }

    m_gamepads.reset();
    m_mouse.reset();
    m_keyboard.reset();
    m_window.reset();

    glfwTerminate();
}

module::module(const config &conf)
    : m_config(conf)
{

}

void module::receive(const message::frame_start&)
{
    engine_cpu_profile(glfw_events);

    glfwPollEvents();
    m_gamepads->update();
}

}
