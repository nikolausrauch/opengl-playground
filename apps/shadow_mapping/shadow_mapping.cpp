#include <cstdlib>

#include <viewer/viewer.h>
#include <viewer/asset/model.h>
#include <viewer/asset/obj_model.h>
#include <viewer/opengl/shaderprogram.h>
#include <viewer/opengl/texture.h>
#include <viewer/opengl/framebuffer.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/euler_angles.hpp>

struct material
{
    float shininess = 32.0;
    opengl::handle<opengl::texture> map_diffuse;
    opengl::handle<opengl::texture> map_specular;
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


/* Directional Light Source */
struct light
{
    glm::vec3 angles = glm::vec3(glm::quarter_pi<float>(), glm::quarter_pi<float>(), glm::quarter_pi<float>());
    glm::vec3 direction = -glm::normalize(  glm::vec3(glm::sin(angles.x) * glm::sin(angles.y),
                                            glm::cos(angles.y),
                                            glm::cos(angles.x) * glm::sin(angles.y)));

    glm::vec3 ambient = {0.4, 0.4, 0.4};
    glm::vec3 color = {0.5, 0.5, 0.5};
};

/* Helper Struct for method controls */
struct shadow
{
    float bias = 0.0025;
    glm::vec2 near_far_plane = {0.0, 30.0};
    float scale = 5.0;
    float distance = 25.0f;

    int resolution = 4*1024;

    int samples = 16;
    float spacing = 0.65f;

    bool face_culling = false;
};

int main(int argc, char** argv)
{
    /* initial window settings */
    viewer::window_settings settings;
    settings.title = "Shadow Mapping";
    settings.width = 1280;
    settings.heigth = 720;

    /* construct viewer (creates window and context) */
    viewer view(settings);

    auto& camera = view.camera();
    camera.position({-2.7, 1.75, -3.2});

    auto& context = view.context();
    light dir_light;
    shadow shadow_settings;

    /* load obj model */
    auto model = asset::model_loader<vertex, material>::load_obj(context, "assets/small_city/small_city.obj");


    /* framebuffer for shadow map */
    auto texture_shadow = context.make_texture(opengl::texture_internal_type::depth32, opengl::texture_format::depth,
                                               opengl::texture_type::float_, shadow_settings.resolution, shadow_settings.resolution);
    texture_shadow->parameter(opengl::wrap_coord::wrap_r, opengl::wrapping::border);
    texture_shadow->parameter(opengl::wrap_coord::wrap_s, opengl::wrapping::border);
    texture_shadow->parameter(opengl::texture_color::border_color, {1.0, 1.0, 1.0, 1.0});

    auto fb_shadow = context.make_framebuffer();
    fb_shadow->attach_depth(texture_shadow);
    fb_shadow->draw_buffer(opengl::color_buffer::none);
    fb_shadow->read_buffer(opengl::color_buffer::none);
    assert(fb_shadow->completed());


    /* create shader and compile */
    auto shader_shadowmap = context.make_shader();
    shader_shadowmap->load("shadow_mapping/shader/shadow_map.vert", opengl::shader_type::vertex);
    shader_shadowmap->load("shadow_mapping/shader/shadow_map.frag", opengl::shader_type::fragment);
    shader_shadowmap->link();

    auto shader_light = context.make_shader();
    shader_light->load("shadow_mapping/shader/blinn_phong.vert", opengl::shader_type::vertex);
    shader_light->load("shadow_mapping/shader/blinn_phong.frag", opengl::shader_type::fragment);
    shader_light->link();


    /* enable/disable OpenGL options */
    context.clear_color(0.0, 0.5, 1.0, 1.0);
    context.set(opengl::options::depth_test, true);
    context.set(opengl::options::cull_face, true);
    context.cull(opengl::polygon_face::back);


    view.on_render([&](auto& window, float dt)
    {
        auto model_matrix = glm::translate(glm::scale(glm::vec3{1.0f, 1.0f, 1.0f} * 0.05f), {0.0f, 2.0f, 0.0f});
        glm::mat4 light_proj = glm::ortho(-shadow_settings.scale, shadow_settings.scale, -shadow_settings.scale, shadow_settings.scale,
                                           shadow_settings.near_far_plane.x, shadow_settings.near_far_plane.y);
        glm::mat4 light_view = glm::lookAt(-shadow_settings.distance * dir_light.direction,
                                          glm::vec3( 0.0f, 0.0f,  0.0f),
                                          glm::vec3( 0.0f, 1.0f,  0.0f));
        glm::mat4 light_matrix = light_proj * light_view;

        /********************** 1. shadow map pass **********************/
        {
            /* backup opengl state */
            auto size = texture_shadow->size();
            auto viewport = context.viewport(0, 0, size.x, size.y);
            auto cull = shadow_settings.face_culling ? context.cull(opengl::polygon_face::front) : opengl::polygon_face::back;

            fb_shadow->bind();
            context.clear(opengl::clear_options::depth);

            shader_shadowmap->bind();
            shader_shadowmap->uniform("uModel", model_matrix);
            shader_shadowmap->uniform("uLightSpace", light_matrix);

            for(const auto& [_, mesh] : model->meshes())
            {
                mesh.vao()->bind();
                mesh.vao()->draw(opengl::primitives::triangles);
            }

            fb_shadow->unbind();

            /* set opengl state */
            context.viewport(viewport);
            context.cull(cull);
        }

        /********************** 2. Blinn-Phong **********************/
        {
            shader_light->bind();
            shader_light->uniform("uModel", model_matrix);
            shader_light->uniform("uView", camera.view());
            shader_light->uniform("uProj", camera.projection());
            shader_light->uniform("uViewPos", camera.position());
            shader_light->uniform("uLightSpace", light_matrix);

            /* light */
            shader_light->uniform("uLight.direction", dir_light.direction);
            shader_light->uniform("uLight.ambient", dir_light.ambient);
            shader_light->uniform("uLight.color", dir_light.color);

            shader_light->uniform("uShadow.bias", shadow_settings.bias);
            shader_light->uniform("uShadow.spacing", shadow_settings.spacing);
            shader_light->uniform("uShadow.samples", shadow_settings.samples);
            shader_light->uniform("uShadow.map_shadow", 2);
            texture_shadow->bind(2);

            shader_light->uniform("uMaterial.map_diffuse", 0);
            shader_light->uniform("uMaterial.map_specular", 1);

            /* iterate over materials */
            for(auto& [_, mat_group] : model->material_groups())
            {
                /* set material uniforms */
                auto& mat = mat_group.material();
                shader_light->uniform("uMaterial.shininess", mat.shininess);
                mat.map_diffuse->bind(0);
                mat.map_specular->bind(1);

                /* iterate over mesh and render faces with this material */
                for(const auto& record : mat_group.records())
                {
                    record.m_mesh.vao()->bind();
                    record.m_mesh.vao()->draw(record.m_offset, record.m_count, opengl::primitives::triangles);
                }
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
            if(ImGui::DragFloat2("direction",  &dir_light.angles[0], 0.01,
                                  -2.0f*glm::pi<float>(),
                                  2.0f*glm::pi<float>()))
            {
                glm::vec3 dir =
                    {
                        glm::sin(dir_light.angles.x) * glm::sin(dir_light.angles.y),
                        glm::cos(dir_light.angles.y),
                        glm::cos(dir_light.angles.x) * glm::sin(dir_light.angles.y)
                    };

                dir_light.direction = glm::normalize(-dir);
            }
            ImGui::ColorEdit3("ambient",  &dir_light.ambient[0]);
            ImGui::ColorEdit3("color",  &dir_light.color[0]);
            ImGui::PopID();


            ImGui::Dummy({0.0, 16.0});


            ImGui::TextColored({1.0, 1.0, 0, 1.0}, "Shadow Map: ");
            ImGui::PushID("shadow");
            ImGui::DragFloat("distance", &shadow_settings.distance);
            ImGui::DragFloat2("Near/Far Plane", &shadow_settings.near_far_plane[0]);
            ImGui::DragFloat("Ortho scale", &shadow_settings.scale);
            ImGui::Checkbox("face culling", &shadow_settings.face_culling);

            ImGui::Separator();

            ImGui::DragFloat("bias", &shadow_settings.bias, 0.00001, 0.0, 0.1, "%.6f");
            ImGui::DragFloat("filter spacing", &shadow_settings.spacing, 0.01, 0.0, 10.0);
            ImGui::DragInt("filter samples", &shadow_settings.samples, 1.0, 1, 64);

            if(ImGui::DragInt("resolution", &shadow_settings.resolution, 1, 1, 8*1024))
            {
                texture_shadow->resize(shadow_settings.resolution, shadow_settings.resolution);
            }
            ImGui::PopID();
        }
        ImGui::End();

        /************** render shadow map **************/
        ImGui::SetNextWindowPos({16.0f, window.size().y - 256.0f - 32.0f});
        ImGui::Begin("##shadowmap", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::Image(texture_shadow.get(), {256, 256}, {0, 1}, {1, 0});
        }
        ImGui::End();
    });

    /* start main loop */
    view.run();

    return EXIT_SUCCESS;
}
