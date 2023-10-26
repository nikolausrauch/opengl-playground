#include <cstdlib>
#include <iostream>

#include <viewer/viewer.h>

#include <viewer/opengl/shaderprogram.h>
#include <viewer/opengl/indexbuffer.h>
#include <viewer/opengl/vertexarray.h>
#include <viewer/opengl/vertexbuffer.h>

#include <glm/gtx/transform.hpp>

/*============= Shader Code =============*/
const char *vertex_shader = GLSL_CODE(330,
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec4 tColor;

void main(void)
{
    gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);
    tColor = aColor;
});

const char *frag_shader = GLSL_CODE(330,
in vec4 tColor;

out vec4 fragColor;

void main(void)
{
    fragColor = tColor;
});

/*============= Vertex Defition =============*/
struct vertex
{
    glm::vec3 position;
    glm::vec4 color;
};

/* vertexbuffer layout definition for OpenGL (i am not really happy with this, but over the years it stuck) */
template <>
struct opengl::layout<vertex>
{
    static constexpr attr_info value[] = {
        {opengl::type::float_, 3, opengl::buffer_mapping::cast, offsetof(vertex, position)},
        {opengl::type::float_, 4, opengl::buffer_mapping::cast, offsetof(vertex, color)}};
};


int main(int argc, char** argv)
{
    /* initial window settings */
    viewer::window_settings settings;
    settings.title = "Colored Cube";
    settings.width = 720;
    settings.heigth = 720;

    /* construct viewer (creates window and context) */
    viewer view(settings);

    /* set camera parameter */
    auto& cam = view.camera();
    cam.position({2.0f, 2.0f, 2.0f});
    cam.look_at({0.0f, 0.0f, 0.0f});

    /* access context and create opengl resources */
    auto& context = view.context();

    /* create vertexarray, and attach vertexbuffer and indexbuffer to it */
    auto vao = context.make_vertexarray();
    auto vertexbuffer = context.make_vertexbuffer<vertex>(
                std::initializer_list<vertex>
                {{{-1.0, -1.0, 1.0}, {1.0, 0.0, 0.0, 1.0}},
                 {{-1.0,  1.0, 1.0}, {0.0, 1.0, 0.0, 1.0}},
                 {{ 1.0,  1.0, 1.0}, {0.0, 0.0, 1.0, 1.0}},
                 {{ 1.0, -1.0, 1.0}, {1.0, 0.0, 1.0, 1.0}},

                 {{-1.0, -1.0, -1.0}, {1.0, 0.0, 0.0, 1.0}},
                 {{-1.0,  1.0, -1.0}, {0.0, 1.0, 0.0, 1.0}},
                 {{ 1.0,  1.0, -1.0}, {0.0, 0.0, 1.0, 1.0}},
                 {{ 1.0, -1.0, -1.0}, {1.0, 0.0, 1.0, 1.0}}});

    auto indexbuffer = context.make_indexbuffer<unsigned int>(
                std::initializer_list<unsigned int>
                {0, 2, 1,
                 2, 0, 3,

                 4, 5, 6,
                 6, 7, 4,

                 0, 1, 5,
                 5, 4, 0,

                 3, 6, 2,
                 6, 3, 7,

                 1, 6, 5,
                 6, 1, 2,

                 0, 4, 7,
                 7, 3, 0});

    vao->attach(vertexbuffer);
    vao->attach(indexbuffer);

    /* create shader and compile */
    auto shader = context.make_shader();
    shader->attach(vertex_shader, opengl::shader_type::vertex);
    shader->attach(frag_shader, opengl::shader_type::fragment);
    shader->link();

    /* enable/disable OpenGL options */
    context.set(opengl::options::depth_test, true);

    view.on_render([&](auto& window, float dt)
    {
        static float s_time = 0.0f;
        s_time += dt;

        /* set shader uniforms */
        shader->bind();
        shader->uniform("uModel", glm::rotate(s_time*glm::pi<float>(), glm::vec3{0.0f, 1.0f, 0.0f}));
        shader->uniform("uView", cam.view());
        shader->uniform("uProj", cam.projection());

        /* draw elements (count is number of primitives) */
        vao->bind();
        context.draw_elements(opengl::primitives::triangles, indexbuffer->size() / 3, opengl::type::unsigned_int_);
    });

    view.on_key([](auto& window, auto key, bool pressed)
    {
        if(key == core::keyboard::key::escape && pressed)
        {
            window.close();
        }

        if(key == core::keyboard::key::f11 && pressed)
        {
            static bool toggle{false};
            toggle = !toggle;
            window.fullscreen(toggle);
        }
    });

    /* start main loop */
    view.run();

    return EXIT_SUCCESS;
}
