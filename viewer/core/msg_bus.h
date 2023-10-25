#pragma once

#include "assert.h"

#include <mutex>
#include <vector>
#include <functional>

namespace core
{

class msg_bus final
{
private:
    template <typename Msg>
    class internal_bus
    {
    private:
        struct listener
        {
            std::function<void(const Msg&)> callback;
            void* raw_ptr;
        };

    private:
        std::vector<listener> m_listener;

    public:
        static internal_bus& instance()
        {
            static internal_bus s_bus;
            return s_bus;
        }

        template<typename Listener>
        void connect(Listener* sink)
        {
            m_listener.emplace_back(listener{[sink](const Msg& msg){ sink->receive(msg); }, static_cast<void*>(sink)});
        }

        template<typename Listener>
        void disconnect(Listener* sink)
        {
            auto it = std::find(m_listener.begin(), m_listener.end(), [sink](const auto& record){ return record.raw_ptr == sink; });
            platform_assert(it != m_listener.end(), "Tried to remove non-existing event listener!");

            if(it != m_listener.end())
            {
                m_listener.erase(it);
            }
        }

        void broadcast(const Msg& msg)
        {
            for(auto& l : m_listener)
            {
                l.callback(msg);
            }
        }
    };

public:
    template<typename Msg, typename Listener>
    void connect(Listener* sink)
    {
        internal_bus<Msg>::instance().connect(sink);
    }

    template<typename Msg, typename Listener>
    void disconnect(Listener* sink)
    {
        internal_bus<Msg>::instance().disconnect(sink);
    }

    template<typename Msg>
    void broadcast(const Msg& msg)
    {
        internal_bus<Msg>::instance().broadcast(msg);
    }

};

}
