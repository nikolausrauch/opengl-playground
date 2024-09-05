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


    /*----------------- read raw --------------------*/
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

volume_loader::result_type volume_loader::load_dat(opengl::context& gl_context, const std::filesystem::path& path)
{
    if (!std::filesystem::is_regular_file(path))
    {
        platform_log(core::log::level::error, "[asset::volume] path does is not a file {0} ", path.string());
        return result_type(new opengl::texture_3D(gl_context, 1, 1, 1));
    }

    std::ifstream file(path, std::ios::binary | std::ios::ate);

    /* get file size */
    file.seekg(0, file.end);
    std::size_t f_size = file.tellg();
    file.seekg(0, file.beg);
    if (f_size == 0)
    {
        file.close();

        platform_log(core::log::level::error, "[asset::volume] Empty volume data at {0}", path.string());
        return result_type(new opengl::texture_3D(gl_context, 1, 1, 1));
    }

    /*----------------- read dat (https://www.cg.tuwien.ac.at/research/vis/datasets/) --------------------*/
    struct __attribute((packed))
    {
        std::uint16_t x;
        std::uint16_t y;
        std::uint16_t z;
    } _header;

    file.read(reinterpret_cast<char*>(&_header), sizeof(_header));
    assert(f_size - sizeof(_header) == _header.x * _header.y * _header.z * 2);

    std::vector<std::uint8_t> data(f_size);
    data.resize(f_size - sizeof(_header));
    file.read(reinterpret_cast<char*>(data.data()), f_size - sizeof(_header));

    auto vol_tex = result_type(new opengl::texture_3D(gl_context,
                                                      opengl::texture_internal_type::r16, opengl::texture_format::red, opengl::texture_type::unsigned_short_,
                                                      _header.x, _header.y, _header.z));
    vol_tex->data(data.data());

    return vol_tex;
}

}
