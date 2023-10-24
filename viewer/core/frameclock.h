#pragma once

#include "viewer/core/time.h"

#include <vector>

namespace core
{

class frameclock
{    
public:
    template<typename T, typename U>
    struct range
    {
        range()
            : m_min()
            , m_max()
            , m_avg()
            , m_current()
            , m_elapsed()
        {}
        void swap(range& other)
        {
            std::swap(this->minimum, other.minimum);
            std::swap(this->maximum, other.maximum);
            std::swap(this->average, other.average);
            std::swap(this->current, other.current);
            std::swap(this->elapsed, other.elapsed);
        }
        T m_min;
        T m_max;
        T m_avg;
        T m_current;
        U m_elapsed;
    };

private:
    time_point m_time_stamp;
    time m_elapsed;

    range<time, time> m_time;
    range<float, unsigned int>  m_freq;

    time m_accum;
    std::vector<time> m_buffer;
    std::vector<time>::size_type m_index;


public:
    frameclock(unsigned int sample_depth);

    void start();
    time restart();

    unsigned int sample_depth() const;
    time dt() const;
    time min() const;
    time max() const;
    time avg() const;

    float fps() const;

private:
    void add(const time dt);
};

}


