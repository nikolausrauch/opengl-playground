#include "texture.h"

#include "image.h"

#include "viewer/core/log.h"

#include <stb_image/stb_image.h>
#include <stb_image/stb_image_write.h>

namespace asset
{

texture_loader::result_type_tex texture_loader::load(opengl::context& gl_context, const std::filesystem::path &path, bool flip)
{
    int width = 1;
    int height = 1;
    int components = 4;

    stbi_set_flip_vertically_on_load(flip);
    unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &components, 4);

    if(data == nullptr)
    {
        platform_log(core::log::level::error, "[asset::texture] Coudn't load texture {0}", path.string());
        return result_type_tex(new opengl::texture(gl_context, 1, 1, asset::color{255, 0, 0, 255}));
    }

    auto* tex = new opengl::texture(gl_context, width, height);
    tex->data(data);

    return result_type_tex(tex);
}

texture_loader::result_type_tex texture_loader::load(opengl::context& context, const image& img)
{
    auto* tex = new opengl::texture(context, img.size().x, img.size().y);
    tex->data(img.ptr());

    return result_type_tex(tex);
}

texture_loader::result_type_tex texture_loader::load(opengl::context& context, unsigned int width, unsigned int height, const color& color)
{
    return result_type_tex(new opengl::texture(context, width, height, color));
}

texture_loader::result_type_cube texture_loader::load_cube(opengl::context& gl_context, const std::array<std::filesystem::path, 6>& paths, bool flip)
{
    int width = 1;
    int height = 1;
    int components = 4;

    stbi_set_flip_vertically_on_load(flip);

    auto* tex = new opengl::texture_cube(gl_context, width, height);
    for(int i = 0; i < paths.size(); i++)
    {
        auto path = paths[i];

        unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &components, 4);

        if(data == nullptr)
        {
            platform_log(core::log::level::error, "[asset::texture] Coudn't load layer of texture cube {0}", path.string());
        }

        tex->data(static_cast<opengl::cube_face>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), data, width, height);
    }

    return result_type_cube(tex);
}

}
