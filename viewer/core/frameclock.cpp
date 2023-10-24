#include "frameclock.h"


namespace core
{

frameclock::frameclock(unsigned int sample_depth)
    : m_time_stamp(clock::now()), m_accum(time::zero()), m_buffer(sample_depth), m_index(0u)
{
    m_freq.m_min = std::numeric_limits<float>::max();
    m_freq.m_max = 0.0f;
    m_time.m_min = time::max();
    m_time.m_max = time::zero();
}

void frameclock::start()
{
    m_time_stamp = clock::now();
}

time frameclock::restart()
{
    auto ending = clock::now();
    m_elapsed = ending - m_time_stamp;
    m_time_stamp = ending;

    add(m_elapsed);

    return m_elapsed;
}

time frameclock::dt() const
{
    return m_elapsed;
}

void frameclock::add(const time dt)
{
    m_time.m_current = dt;

    m_accum -= m_buffer[m_index];
    m_buffer[m_index] = m_time.m_current;

    m_accum += m_time.m_current;
    m_time.m_elapsed += m_time.m_current;

    if(++m_index >= m_buffer.size())
    {
        m_index = 0;
    }

    if(m_time.m_current != time::zero())
    {
        m_freq.m_current = 1.0f / time_cast<sec>(m_time.m_current);
    }

    if(m_accum != time::zero())
    {
        m_freq.m_avg = static_cast<float>(m_buffer.size()) / time_cast<sec>(m_accum);
    }

    m_time.m_avg = m_accum / m_buffer.size();

    m_freq.m_min = std::min(m_freq.m_min, m_freq.m_current);
    m_freq.m_max = std::max(m_freq.m_max, m_freq.m_current);

    m_time.m_min = std::min(m_time.m_min, m_time.m_current);
    m_time.m_max = std::max(m_time.m_max, m_time.m_current);

    m_freq.m_elapsed++;
}

}
