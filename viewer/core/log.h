#pragma once

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <functional>
#include <list>
#include <string>
#include <source_location>

namespace core
{

class log
{
public:
    enum class level : int
    {
        msg = 0,
        info,
        warning,
        error
    };

    struct message
    {
        const std::string file;
        const unsigned int line;
        const std::string func;
        const std::string text;
    };
    using Sink = std::function< void(const message&) >;

private:
    std::array< std::list<Sink>, static_cast<int>(level::error) + 1 > m_sinks;


public:
    static log& instance();
    void add_sink(const level l, const Sink& sink);
    void message(const level l, const std::string& text, const std::source_location& loc = std::source_location::current());
    void clear();

private:
    log() = default;
    log(const log&) = delete;
    log& operator=(const log&) = delete;
};

#if defined(PLATFORM_LOG)
    static constexpr bool g_logging = true;
    #define platform_log(level, ...) core::log::instance().message(level, fmt::format(__VA_ARGS__))
#else
    static constexpr bool g_logging = false;
    #define platform_log(level, ...)
#endif

}


