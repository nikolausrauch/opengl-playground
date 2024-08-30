#include "volume.h"

#include "viewer/core/log.h"

namespace asset
{

volume_loader::result_type volume_loader::load_raw(opengl::context& gl_context, const std::filesystem::path& path, const glm::uvec3& size)
{
    if (!std::filesystem::is_regular_file(path))
    {
        platform_log(core::log::level::error, "[asset::volume] path does is not a file {0} ", path.string());
        return result_type(new opengl::texture_3D(gl_context, 1, 1, 1));
    }

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!std::filesystem::is_regular_file(path))
    {
        platform_log(core::log::level::error, "[asset::volume] Coudn't load raw volume {0}", path.string());
        return result_type(new opengl::texture_3D(gl_context, 1, 1, 1));
    }

    /* get file size */
    file.seekg(0, file.end);
    auto f_size = file.tellg();
    file.seekg(0, file.beg);
    if (f_size == 0)
    {
        file.close();

        platform_log(core::log::level::error, "[asset::volume] Empty volume data at {0}", path.string());
        return result_type(new opengl::texture_3D(gl_context, 1, 1, 1));
    }

    if( f_size != size.x*size.y*size.z )
    {
        file.close();

        platform_log(core::log::level::error, "[asset::volume] Volume size does not match specified size (voxel size is assumed to be one byte) {0}", path.string());
        return result_type(new opengl::texture_3D(gl_context, 1, 1, 1));
    }

    std::vector<std::uint8_t> data(f_size);
    file.read(reinterpret_cast<char*>(data.data()), data.size());

    auto vol_tex = result_type(new opengl::texture_3D(gl_context,
                                                      opengl::texture_internal_type::r16, opengl::texture_format::red, opengl::texture_type::unsigned_byte_,
                                                      size.x, size.y, size.z));
    vol_tex->data(data.data());

    vol_tex->parameter(opengl::wrap_coord::wrap_s, opengl::wrapping::edge);
    vol_tex->parameter(opengl::wrap_coord::wrap_t, opengl::wrapping::edge);
    vol_tex->parameter(opengl::wrap_coord::wrap_r, opengl::wrapping::edge);

    return vol_tex;
}

}
