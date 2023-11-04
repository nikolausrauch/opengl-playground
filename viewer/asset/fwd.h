#pragma once

#include <memory>

namespace asset
{

template<typename T> using handle = std::shared_ptr<T>;

class image;
class image_loader;

class texture_loader;

}
