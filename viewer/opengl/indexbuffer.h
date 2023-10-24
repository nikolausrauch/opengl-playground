#pragma once

#include "buffer.h"

namespace opengl
{

template <typename T>
class indexbuffer : public buffer<T>
{
private:
    indexbuffer(context& context, buffer_usage usage = buffer_usage::static_draw);
    indexbuffer(context& context, size_t numElements, buffer_usage usage = buffer_usage::static_draw);
    indexbuffer(context& context, std::initializer_list<T> items, buffer_usage usage = buffer_usage::static_draw);
    indexbuffer(context& context, const std::vector<T>& items, buffer_usage usage = buffer_usage::static_draw);
    indexbuffer(context& context, const T* items, std::size_t numElement, buffer_usage usage = buffer_usage::static_draw);

    friend context;
};

template <typename T>
indexbuffer<T>::indexbuffer(context& context, buffer_usage usage)
    : buffer<T>(context, buffer_target::element_array, usage)
{

}

template <typename T>
indexbuffer<T>::indexbuffer(context& context, size_t numElements, buffer_usage usage)
    : buffer<T>(context, buffer_target::element_array, numElements, usage)
{

}

template <typename T>
indexbuffer<T>::indexbuffer(context& context, std::initializer_list<T> items, buffer_usage usage)
    : buffer<T>(context, buffer_target::element_array, std::forward<std::initializer_list<T>>(items), usage)
{

}

template <typename T>
indexbuffer<T>::indexbuffer(context& context, const std::vector<T> &items, buffer_usage usage)
    : buffer<T>(context, buffer_target::element_array, items, usage)
{

}

template <typename T>
indexbuffer<T>::indexbuffer(context& context, const T* items, std::size_t number, buffer_usage usage)
    : buffer<T>(context, buffer_target::element_array, items, number , usage)
{

}

}
