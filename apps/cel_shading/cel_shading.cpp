#include <cstdlib>

#include <viewer/viewer.h>
#include <viewer/asset/model.h>
#include <viewer/asset/obj_model.h>
#include <viewer/asset/shapes.h>
#include <viewer/opengl/shaderprogram.h>
#include <viewer/opengl/framebuffer.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/euler_angles.hpp>

struct material
{
    float shininess;
    glm::vec3 specular;

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
        {opengl::type::float_, 2, opengl::buffer_mapping::cast, offsetof(vertex, texcoord)}
    };
};



struct vertex_screen
{
    glm::vec3 position;
    glm::vec2 texcoord;
};

/* vertexbuffer layout definition for OpenGL */
template <>
struct opengl::layout<vertex_screen>
{
    static constexpr attr_info value[] = {
        {opengl::type::float_, 3, opengl::buffer_mapping::cast, offsetof(vertex_screen, position)},
        {opengl::type::float_, 2, opengl::buffer_mapping::cast, offsetof(vertex_screen, texcoord)}
    };
};

opengl::handle<opengl::texture> create_gradient_map(opengl::context& context, unsigned int buckets)
{
    std::vector<unsigned char> intensities(buckets, 0);
    for(auto i = 0u; i < intensities.size(); i++)
    {
        intensities[i] = static_cast<unsigned char>((255 * i) / buckets);
    }

    auto tex = context.make_texture(opengl::texture_internal_type::r8,
                                    opengl::texture_format::red,
                                    opengl::texture_type::unsigned_byte_, buckets, 1);
    tex->data(intensities.data());
    tex->parameter(opengl::mag_filter::nearest);
    tex->parameter(opengl::min_filter::nearest);

    return tex;
}


/* Directional Light Source */
struct light
{
    glm::vec3 angles = glm::vec3(glm::quarter_pi<float>(), glm::quarter_pi<float>(), glm::quarter_pi<float>());
    glm::vec3 direction = -glm::normalize(  glm::vec3(glm::sin(angles.x) * glm::sin(angles.y),
                                                    glm::cos(angles.y),
                                                    glm::cos(angles.x) * glm::sin(angles.y)));

    glm::vec3 ambient = {0.4, 0.4, 0.4};
    glm::vec3 color = {1.0, 1.0, 1.0};
};


struct cel_shading
{
    float step_size = 2.0f;
    float threshold = 0.1f;
    int buckets = 3;

    opengl::handle<opengl::texture> gradient;
};

int main(int argc, char** argv)
{
    /* initial window settings */
    viewer::window_settings settings;
    settings.title = "Cel Shading";
    settings.width = 1280;
    settings.heigth = 720;

    /* construct viewer (creates window and context) */
    viewer view(settings);

    /* set camera position */
    auto& camera = view.camera();
    camera.position({0.5, 0.1, 0.5});
    camera.clip_planes(0.1, 3.0);

    auto& context = view.context();

    /* load obj model */
    auto model = asset::model_loader<vertex, material>::load_obj(context, "assets/bird/bird.obj");
    auto screen_quad = asset::shape<vertex_screen>::create_screenquad(context);

    /* framebuffer for cel-shaded color and depth values (needed for edge-detection) */
    auto tex_color = context.make_texture(opengl::texture_internal_type::rgba8, opengl::texture_format::rgba,
                                          opengl::texture_type::unsigned_byte_, settings.width, settings.heigth);
    tex_color->parameter(opengl::min_filter::nearest);
    tex_color->parameter(opengl::mag_filter::nearest);

    auto tex_depth = context.make_texture(opengl::texture_internal_type::depth32, opengl::texture_format::depth,
                                          opengl::texture_type::unsigned_int_, settings.width, settings.heigth);
    tex_depth->parameter(opengl::min_filter::nearest);
    tex_depth->parameter(opengl::mag_filter::nearest);
    tex_depth->parameter(opengl::wrap_coord::wrap_s, opengl::wrapping::edge);
    tex_depth->parameter(opengl::wrap_coord::wrap_t, opengl::wrapping::edge);

    auto fb_shading = context.make_framebuffer();
    fb_shading->attach_color(0, tex_color);
    fb_shading->attach_depth(tex_depth);
    fb_shading->draw_attachment(0);
    assert(fb_shading->completed());


    /* helper structures */
    light light_dir;
    cel_shading cel_data;
    cel_data.gradient = create_gradient_map(context,  cel_data.buckets);


    /* create shader and compile */
    auto shader_scene = context.make_shader();
    shader_scene->load("cel_shading/shader/cel_shading.vert", opengl::shader_type::vertex);
    shader_scene->load("cel_shading/shader/cel_shading.frag", opengl::shader_type::fragment);
    shader_scene->link();

    auto shader_edge_detect = context.make_shader();
    shader_edge_detect->load("cel_shading/shader/basic.vert", opengl::shader_type::vertex);
    shader_edge_detect->load("cel_shading/shader/edge_detect.frag", opengl::shader_type::fragment);
    shader_edge_detect->link();


    /* OpenGL options */
    context.set(opengl::options::depth_test, true);
    context.clear_color(1.0, 1.0, 1.0, 1.0);


    /***************** install callbacks *****************/
    view.on_render([&](auto& window, float dt)
    {
        auto model_matrix = glm::translate(glm::scale(glm::mat4(1.0), glm::vec3{1.0, 1.0, 1.0} * 0.4f), glm::vec3{0.0, 0.0, 0.0});

        /********************** 1. cel shading **********************/
        fb_shading->bind();
        context.clear(opengl::clear_options::color_depth);
        {
            shader_scene->bind();
            shader_scene->uniform("uModel", model_matrix);
            shader_scene->uniform("uView", camera.view());
            shader_scene->uniform("uProj", camera.projection());
            shader_scene->uniform("uViewPos", camera.position());

            shader_scene->uniform("uLight.direction", light_dir.direction);
            shader_scene->uniform("uLight.ambient", light_dir.ambient);
            shader_scene->uniform("uLight.color", light_dir.color);

            /* iterate over materials */
            for(auto& [_, mat_group] : model->material_groups())
            {
                /* set material uniforms */
                auto& mat = mat_group.material();
                shader_scene->uniform("uMaterial.shininess", mat.shininess);
                shader_scene->uniform("uMaterial.specular", mat.specular);
                shader_scene->uniform("uMaterial.map_diffuse", 1);
                mat_group.material().map_diffuse->bind(1);

                /* set intensity bucket map */
                shader_scene->uniform("intensity_map", 0);
                cel_data.gradient->bind(0);

                /* iterate over mesh and render faces with this material */
                for(const auto& record : mat_group.records())
                {
                    record.m_mesh.vao()->bind();
                    record.m_mesh.vao()->draw(record.m_offset, record.m_count, opengl::primitives::triangles);
                }
            }
        }
        fb_shading->unbind();


        /******************** 2. edge detection ********************/
        {
            shader_edge_detect->bind();
            shader_edge_detect->uniform("uNearFar", glm::vec2( camera.near_plane(), camera.far_plane() ));
            shader_edge_detect->uniform("uColorTexture", 0);
            shader_edge_detect->uniform("uDepthTexture", 1);
            shader_edge_detect->uniform("uStepSize", cel_data.step_size);
            shader_edge_detect->uniform("uThreshold", cel_data.threshold);

            tex_color->bind(0);
            tex_depth->bind(1);

            screen_quad->vao()->draw(opengl::primitives::triangles);
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

    view.on_resize([&](auto& window, unsigned int w, unsigned int h)
    {
        tex_color->resize(w, h);
        tex_depth->resize(w, h);
    });

    view.on_gui([&](auto& window, float dt)
    {
        /************** settings **************/
        ImGui::SetNextWindowPos({16, 16});
        ImGui::Begin("##Settings", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::TextColored({1.0, 1.0, 1.0, 1.0}, "FPS:       %4.2f        ", view.frameclock().fps());


            ImGui::Dummy({0.0, 16.0});


            ImGui::TextColored({1.0, 1.0, 0, 1.0}, "Light: ");
            ImGui::PushID("light");
            if(ImGui::DragFloat2("direction",  &light_dir.angles[0], 0.01,
                                  -2.0f*glm::pi<float>(),
                                  2.0f*glm::pi<float>()))
            {
                glm::vec3 dir =
                    {
                        glm::sin(light_dir.angles.x) * glm::sin(light_dir.angles.y),
                        glm::cos(light_dir.angles.y),
                        glm::cos(light_dir.angles.x) * glm::sin(light_dir.angles.y)
                    };

                light_dir.direction = glm::normalize(-dir);
            }
            ImGui::ColorEdit3("ambient",  &light_dir.ambient[0]);
            ImGui::ColorEdit3("color",  &light_dir.color[0]);
            ImGui::PopID();


            ImGui::Dummy({0.0, 16.0});


            ImGui::TextColored({1.0, 1.0, 0, 1.0}, "Edge Detection: ");
            ImGui::PushID("edge_detect");

            ImGui::SliderFloat("pixel_size", &cel_data.step_size, 0.0, 16.0, "%.1f");
            ImGui::DragFloat("threshold", &cel_data.threshold, 0.01, 0.0, 4.0, "%.3f");

            if(ImGui::SliderInt("buckets", &cel_data.buckets, 1, 16))
            {
                cel_data.gradient = create_gradient_map(context,  cel_data.buckets);
            }

            ImGui::PopID();
        }
        ImGui::End();
    });

    /* start main loop */
    view.run();

    return EXIT_SUCCESS;
}
