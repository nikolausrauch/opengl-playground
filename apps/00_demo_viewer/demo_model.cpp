#include <cstdlib>

#include <viewer/viewer.h>
#include <viewer/asset/model.h>
#include <viewer/asset/obj_model.h>
#include <viewer/opengl/shaderprogram.h>
#include <viewer/opengl/texture.h>

#include <glm/gtx/transform.hpp>

/*============= Shader Code =============*/
const char *vertex_shader = GLSL_CODE(330,
    layout(location = 0) in vec3 aPosition;
    layout(location = 1) in vec3 aNormal;
    layout(location = 2) in vec2 aUV;

    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProj;

    out vec3 tNormal;
    out vec2 tUV;

    void main(void)
    {
        gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);
        tNormal = aNormal;
        tUV = aUV;
    });


const char *frag_shader = GLSL_CODE(330,
    in vec3 tNormal;
    in vec2 tUV;

    out vec4 fragColor;

    uniform sampler2D uMapDiffuse;

    void main(void)
    {
        fragColor = texture(uMapDiffuse, tUV);
    });


struct material
{
    opengl::handle<opengl::texture> map_diffuse;
};

struct vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
};

/* vertexbuffer layout definition for OpenGL */
template <>
struct opengl::layout<vertex>
{
    static constexpr attr_info value[] = {
        {opengl::type::float_, 3, opengl::buffer_mapping::cast, offsetof(vertex, position)},
        {opengl::type::float_, 3, opengl::buffer_mapping::cast, offsetof(vertex, normal)},
        {opengl::type::float_, 2, opengl::buffer_mapping::cast, offsetof(vertex, texcoord)}};
};


int main(int argc, char** argv)
{
    /* initial window settings */
    viewer::window_settings settings;
    settings.title = "Model Loading";
    settings.width = 1280;
    settings.heigth = 720;

    /* construct viewer (creates window and context) */
    viewer view(settings);

    /* set camera position */
    auto& camera = view.camera();
    camera.position({1.75, 0.0, 0.0f});

    /* load model with specified vertex and material definitions (--> see asset/model.h for all possible member) */
    auto& context = view.context();
    auto model = asset::model_loader<vertex, material>::load_obj(context, "assets/sad_toaster/sad_toaster.obj");

    /* create shader and compile */
    auto shader = context.make_shader();
    shader->attach(vertex_shader, opengl::shader_type::vertex);
    shader->attach(frag_shader, opengl::shader_type::fragment);
    shader->link();

    /* enable/disable OpenGL options */
    context.set(opengl::options::depth_test, true);

    /***************** install callbacks *****************/
    view.on_render([&](auto& window, float dt)
    {
        shader->bind();
        shader->uniform("uModel", glm::translate(glm::mat4(1.0), {0.0f, -1.0f, 0.0f}));
        shader->uniform("uView", camera.view());
        shader->uniform("uProj", camera.projection());

        /* iterate over materials */
        for(auto& [_, mat_group] : model->material_groups())
        {
            /* set material uniforms */
            auto& mat = mat_group.material();
            shader->uniform("uMapDiffuse", 0);
            mat.map_diffuse->bind(0);

            /* iterate over mesh and render faces with this material */
            for(const auto& record : mat_group.records())
            {
                record.m_mesh.get().vao()->bind();
                record.m_mesh.get().vao()->draw(record.m_offset, record.m_count, opengl::primitives::triangles);
            }
        }
    });

    view.on_key([](auto& window, auto key, bool pressed)
    {
        /* exit on escape */
        if(key == core::keyboard::key::escape && pressed)
        {
            window.close();
        }

        /* toggle fullscreen window */
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
