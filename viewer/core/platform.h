#pragma once


/********** SYSTEM ***********/
enum class osystem
{
    windows,
    macosx,
    linux
};

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    static constexpr osystem g_system = osystem::windows;
    #define PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #error "Platform [APPLE] not implemented!"

    #include <TargetConditionals.h>
    #if TARGET_OS_MAC
        static constexpr system g_system = system::macosx;
    #else
        #error "Unknown Apple platform"
    #endif

#elif defined(__linux__)
    static constexpr system g_system = system::linux;
    #define PLATFORM_LINUX

#elif
    #error "Unknown Platform"
#endif


/********** GRAPHICS API ***********/
enum class graphic_api
{
    opengl
};

static constexpr graphic_api g_render_api = graphic_api::opengl;


/********** DEBUG ***********/
#if defined(PLATFORM_DEBUG)
    static constexpr bool g_debug = true;
    #if defined(PLATFORM_WINDOWS)
        #define platform_break() __debugbreak();

    #elif defined(PLATFORM_LINUX)
        #include <signal.h>
        #define platform_break() raise(SIGTRAP)

    #endif

#else
    static constexpr bool g_debug = false;

#endif

