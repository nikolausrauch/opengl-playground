#include "camera.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace util
{

camera::camera(float width, float heigth, float fov, float near_plane, float far_plane)
    : m_width(width), m_height(heigth), m_near_plane(near_plane), m_far_plane(far_plane), m_fov(fov)
{

}

void camera::perspective(float width, float height, float fov, float near_clip, float far_clip)
{
    m_mode = mode::perspective;
    m_fov = fov;
    m_width = width;
    m_height = height;
}

void camera::ortho(float width, float height, float near_clip, float far_clip)
{
    m_mode = mode::ortho;
    m_width = width;
    m_height = height;
}

void camera::look_at(const glm::vec3& p)
{
    m_lookat = p;
}

const glm::vec3& camera::look_at() const
{
    return m_lookat;
}

void camera::position(const glm::vec3& pos)
{
    m_position = pos;
}

const glm::vec3& camera::position() const
{
    return m_position;
}

void camera::fov(float radians)
{
    m_fov = radians;
}

float camera::fov() const
{
    return m_fov;
}

void camera::size(float width, float height)
{
    m_width = width;
    m_height = height;
}

float camera::width() const
{
    return m_width;
}

float camera::height() const
{
    return m_height;
}

float camera::aspect() const
{
    return static_cast<float>(m_width) / m_height;
}

void camera::clip_planes(float near, float far)
{
    m_near_plane = near;
    m_far_plane = far;
}

float camera::near_plane() const
{
    return m_near_plane;
}

float camera::far_plane() const
{
    return m_far_plane;
}

const glm::mat4 camera::view() const
{
    return glm::lookAt(m_position, m_lookat, m_initup) * glm::translate(glm::mat4(1.0), -m_position);
}

const glm::mat4 camera::projection() const
{
    return m_mode == mode::perspective ?
                glm::perspective(m_fov, aspect(), m_near_plane, m_far_plane)
              : glm::ortho(0.0f, m_width, 0.0f, m_height, m_near_plane, m_far_plane);
}

}
