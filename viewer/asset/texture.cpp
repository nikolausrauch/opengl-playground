#include "texture.h"

#include "viewer/core/log.h"

#include "viewer/opengl/texture.h"

#include <stb_image/stb_image.h>
#include <stb_image/stb_image_write.h>

namespace asset
{

texture_loader::result_type texture_loader::operator()(opengl::context& gl_context, const std::filesystem::path &path)
{
    int width = 1;
    int height = 1;
    int components = 4;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &components, 4);

    if(data == nullptr)
    {
        platform_log(core::log::level::error, "[asset::texture] Coudn't load texture {0}", path.string());
        return result_type(new opengl::texture(gl_context, 1, 1, asset::color{255, 0, 0, 255}));
    }

    auto* tex = new opengl::texture(gl_context, width, height);
    tex->data(data);

    return result_type(tex);
}

}
