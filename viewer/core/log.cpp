#include "log.h"

namespace core
{

log &log::instance()
{
    static log s_log;
    return s_log;
}

void log::add_sink(const level l, const Sink &sink)
{
    m_sinks[static_cast<int>(l)].emplace_back(sink);
}

void log::message(const level l, const std::string &text, const std::source_location &loc)
{
    struct log::message msg{ .file = loc.file_name(), .line = loc.line(), .func = loc.function_name(), .text = text };
    for(auto& sinks : m_sinks[static_cast<int>(l)])
    {
        sinks(msg);
    }
}

void log::clear()
{
    for(auto& sinks : m_sinks)
    {
        sinks.clear();
    }
}

}
