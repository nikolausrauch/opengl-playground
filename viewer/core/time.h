#pragma once

#include <chrono>

namespace core
{

typedef std::conditional<std::chrono::high_resolution_clock::is_steady,
        std::chrono::high_resolution_clock, std::chrono::steady_clock >::type clock;

typedef clock::time_point    time_point;
typedef clock::duration      time;

typedef std::chrono::nanoseconds    nano_sec;
typedef std::chrono::microseconds   micro_sec;
typedef std::chrono::milliseconds   milli_sec;
typedef std::chrono::seconds        sec;


template <typename T>
inline double time_cast(const time& time)
{
    return std::chrono::duration_cast<T>(time).count();
}

template<>
inline double time_cast<nano_sec>(const time& t)
{
    return std::chrono::duration_cast<nano_sec>(t).count();
}

template<>
inline double time_cast<micro_sec>(const time& t)
{
    return std::chrono::duration_cast<micro_sec>(t).count();
}

template<>
inline double time_cast<milli_sec>(const time& t)
{
    return std::chrono::duration<double, std::ratio<1, 1000>>(t).count();
}

template<>
inline double time_cast<sec>(const time& t)
{
    return std::chrono::duration<double>(t).count();
}

}
