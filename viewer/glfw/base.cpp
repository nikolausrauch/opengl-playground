#include "base.h"

#include "viewer/core/assert.h"
#include "viewer/core/log.h"

#include <string>

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
    platform_log(core::log::level::error, "GLFW error ({0}) : {1}", error, description);
}

}

void startup()
{
    glfwSetErrorCallback(&detail::glfw_error_callback);
    if(!glfwInit())
    {
        platform_assert(false, "Failed to initialize GLFW");
        throw std::runtime_error("failed to initialize GLFW");
    }
}

void shutdown()
{
    glfwTerminate();
}

void poll_events()
{
    glfwPollEvents();
}

}
