#include <cstdlib>

#include <viewer/viewer.h>

#include <viewer/opengl/shaderprogram.h>
#include <viewer/opengl/indexbuffer.h>
#include <viewer/opengl/vertexarray.h>
#include <viewer/opengl/vertexbuffer.h>

/*============= Shader Code =============*/
const char *vertex_shader = GLSL_CODE(330,
    layout(location = 0) in vec3 aPosition;
    layout(location = 1) in vec4 aColor;

    out vec4 tColor;

    void main(void)
    {
        gl_Position = vec4(aPosition, 1.0);
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

/* vertexbuffer layout definition for OpenGL */
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
    settings.title = "Colored Triangle";
    settings.width = 720;
    settings.heigth = 720;

    /* construct viewer (creates window and context) */
    viewer view(settings);

    /* access context and create opengl resources */
    auto& context = view.context();

    /* create vertexarray, and attach vertexbuffer to it */
    auto vao = context.make_vertexarray();
    auto vertexbuffer = context.make_vertexbuffer<vertex>(
                std::initializer_list<vertex>
                {{ {-0.5, -0.5, 0.5}, {1.0, 0.0, 0.0, 1.0} },
                 { { 0.5, -0.5, 0.5}, {0.0, 1.0, 0.0, 1.0} },
                 { { 0.0,  0.5, 0.5}, {0.0, 0.0, 1.0, 1.0} } });
    vao->attach(vertexbuffer);

    /* create shader and compile */
    auto shader = context.make_shader();
    shader->attach(vertex_shader, opengl::shader_type::vertex);
    shader->attach(frag_shader, opengl::shader_type::fragment);
    shader->link();

    view.on_render([&](auto& window, double dt)
    {
        shader->bind();

        /* draw vertexbuffer content */
        vao->draw(opengl::primitives::triangles);
    });

    view.on_key([](auto& window, auto key, bool pressed)
    {
        if(key == core::keyboard::key::escape && pressed)
        {
            window.close();
        }
    });

    /* start main loop */
    view.run();

    return EXIT_SUCCESS;
}
