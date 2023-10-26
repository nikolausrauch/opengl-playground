#pragma once

#include <glm/ext/scalar_constants.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace util
{

class camera
{
public:
    enum class mode
    {
        perspective,
        ortho
    };


private:
    mode m_mode{mode::perspective};

    glm::vec3 m_position{0, 0, 1};
    glm::vec3 m_lookat{0, 0, 0};
    glm::vec3 m_initup{0, 1, 0};

    float m_width{1280.0f};
    float m_height{720.0f};
    float m_near_plane{0.01f};
    float m_far_plane{20.0f};
    float m_fov{0.25f*glm::pi<float>()};


public:
    camera() = default;
    camera(float width, float heigth, float fov, float near_plane, float far_plane);

    void perspective(float width, float heigth, float fov, float near_clip, float far_clip);
    void ortho(float width, float height, float near_clip, float far_clip);

    void look_at(const glm::vec3 &p);
    const glm::vec3& look_at() const;

    void position(const glm::vec3 &pos);
    const glm::vec3& position() const;

    void fov(float radians);
    float fov() const;

    void size(float width, float height);
    float width() const;
    float height() const;
    float aspect() const;

    void clip_planes(float near, float far);
    float near_plane() const;
    float far_plane() const;

    const glm::mat4 view() const;
    const glm::mat4 projection() const;
};

}
