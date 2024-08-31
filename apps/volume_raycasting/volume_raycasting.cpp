#include <cstdlib>

#include <viewer/viewer.h>
#include <viewer/asset/volume.h>
#include <viewer/asset/shapes.h>
#include <viewer/asset/texture.h>
#include <viewer/opengl/shaderprogram.h>
#include <viewer/opengl/framebuffer.h>

#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>

struct vertex
{
    glm::vec3 position;
};

/* vertexbuffer layout definition for OpenGL */
template <>
struct opengl::layout<vertex>
{
    static constexpr attr_info value[] = {
        {opengl::type::float_, 3, opengl::buffer_mapping::cast, offsetof(vertex, position)}
    };
};


/* Directional Light Source */
struct light
{
    glm::vec3 angles = glm::vec3(glm::quarter_pi<float>(), glm::quarter_pi<float>(), glm::quarter_pi<float>());
    glm::vec3 direction = -glm::normalize(  glm::vec3(glm::sin(angles.x) * glm::sin(angles.y),
                                                    glm::cos(angles.y),
                                                    glm::cos(angles.x) * glm::sin(angles.y)));

    glm::vec3 ambient = {0.2, 0.2, 0.2};
    glm::vec3 color = {1.0, 1.0, 1.0};
};

/* helper */
struct raycasting
{
    float step_size = 0.005f;
    int max_steps = 4*256;

    int render_type = 1;
    float iso_value = 0.15f;
    float gamma = 1.5f;
};

int main(int argc, char** argv)
{
    /* initial window settings */
    viewer::window_settings settings;
    settings.title = "Volume Raycasting";
    settings.width = 1280;
    settings.heigth = 720;

    raycasting raycast_settings;
    light light_dir;

    /* construct viewer (creates window and context) */
    viewer view(settings);

    /* set camera position */
    auto& camera = view.camera();
    camera.position({-1.0, 0.0, 0.0});
    camera.clip_planes(0.1, 10.0);

    auto& context = view.context();

    auto tex_volume = asset::volume_loader::load_raw(context, "assets/skull/skull_256x256x256_uint8.raw", {256, 256, 256});
    auto mesh_cube = asset::shape<vertex>::create_unitcube(context);

    /* create entry exit texture framebuffer */
    auto tex_entry = context.make_texture(opengl::texture_internal_type::rgb16, opengl::texture_format::rgb,
                                          opengl::texture_type::unsigned_short_, settings.width, settings.heigth);

    auto tex_exit = context.make_texture(opengl::texture_internal_type::rgb16, opengl::texture_format::rgb,
                                          opengl::texture_type::unsigned_short_, settings.width, settings.heigth);

    auto fb_entry_exit = context.make_framebuffer();
    fb_entry_exit->attach_color(0, tex_entry);
    fb_entry_exit->attach_color(1, tex_exit);
    fb_entry_exit->draw_attachment(0, 1);
    assert(fb_entry_exit->completed());


    /* create shader and compile */
    auto shader_entry_exit = context.make_shader();
    shader_entry_exit->load("volume_raycasting/shader/entry_exit.vert", opengl::shader_type::vertex);
    shader_entry_exit->load("volume_raycasting/shader/entry_exit.frag", opengl::shader_type::fragment);
    shader_entry_exit->link();

    auto shader_raycast = context.make_shader();
    shader_raycast->load("volume_raycasting/shader/simple.vert", opengl::shader_type::vertex);
    shader_raycast->load("volume_raycasting/shader/raycast.frag", opengl::shader_type::fragment);
    shader_raycast->link();

    /* opengl context settings */
    context.clear_color(0, 0, 0, 1);
    context.set(opengl::options::blend, true);
    context.set(opengl::blend_func_factor_alpha::src_alpha, opengl::blend_func_factor_alpha::one_minus_src_alpha);


    /***************** install callbacks *****************/
    view.on_render([&](auto& window, float dt)
    {
        auto model = glm::rotate(glm::mat4(1.0), -glm::half_pi<float>(), glm::vec3{1.0, 0.0, 0.0});

        /* 1. render pass, computing entry exit points */
        fb_entry_exit->bind();
        {
            context.clear(opengl::clear_options::color);

            auto viewport = context.viewport(0, 0, tex_entry->size().x, tex_entry->size().y);
            auto cull_face = context.set(opengl::options::cull_face, false);
            auto blend = context.set(opengl::options::blend, true);
            auto blend_func = context.set(opengl::blend_func_factor_alpha::one, opengl::blend_func_factor_alpha::one);

            shader_entry_exit->bind();
            shader_entry_exit->uniform("uModel", model);
            shader_entry_exit->uniform("uView", camera.view());
            shader_entry_exit->uniform("uProj", camera.projection());

            mesh_cube->vao()->draw(opengl::primitives::triangles);

            shader_entry_exit->unbind();

            context.viewport(viewport);
            context.set(opengl::options::cull_face, cull_face);
            context.set(opengl::options::blend, blend);
            context.set(blend_func.first, blend_func.second);
        }
        fb_entry_exit->unbind();

        /* 2. render pass, perform raycasting */
        {
            shader_raycast->bind();
            shader_raycast->uniform("uModel", model);
            shader_raycast->uniform("uView", camera.view());
            shader_raycast->uniform("uProj", camera.projection());
            shader_raycast->uniform("uScreenSize", glm::vec2(window.size().x, window.size().y));

            shader_raycast->uniform("uRaycast.entry", 0);
            shader_raycast->uniform("uRaycast.exit", 1);
            shader_raycast->uniform("uRaycast.volume", 2);
            shader_raycast->uniform("uRaycast.stepSize", raycast_settings.step_size);
            shader_raycast->uniform("uRaycast.maxSteps", raycast_settings.max_steps);

            shader_raycast->uniform("uRaycast.renderType", raycast_settings.render_type);
            shader_raycast->uniform("uRaycast.isoValue", raycast_settings.iso_value);
            shader_raycast->uniform("uRaycast.gamma", raycast_settings.gamma);

            shader_raycast->uniform("uLight.direction", light_dir.direction);
            shader_raycast->uniform("uLight.ambient", light_dir.ambient);
            shader_raycast->uniform("uLight.color", light_dir.color);

            tex_entry->bind(0);
            tex_exit->bind(1);
            tex_volume->bind(2);

            mesh_cube->vao()->draw(opengl::primitives::triangles);

            tex_entry->unbind();
            tex_exit->unbind();
            tex_volume->unbind();
        }
    });

    view.on_resize([&](auto& window, unsigned int width, unsigned int height)
    {
        tex_entry->resize(width, height);
        tex_exit->resize(width, height);
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
        }

        ImGui::TextColored({1.0, 1.0, 0, 1.0}, "Raycast: ");
        ImGui::PushID("raycast");
        ImGui::DragFloat("step_size", &raycast_settings.step_size, 0.0001, 0.0, 1.0);
        ImGui::DragInt("max_steps", &raycast_settings.max_steps, 1, 0, 1024*4);

        ImGui::Separator();

        ImGui::RadioButton("ISO", &raycast_settings.render_type, 0); ImGui::SameLine();
        ImGui::RadioButton("MIP", &raycast_settings.render_type, 1);

        ImGui::DragFloat("iso_value", &raycast_settings.iso_value, 0.0001, 0.0, 1.0);

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

        ImGui::Separator();

        ImGui::TextColored({1.0, 1.0, 0, 1.0}, "Entry / Exit Texture");
        {
            glm::vec2 size = tex_entry->size() / 5u;
            ImGui::Image(tex_entry.get(), {size.x, size.y}, {0, 1}, {1, 0});
            ImGui::Image(tex_exit.get(), {size.x, size.y}, {0, 1}, {1, 0});
        }

        ImGui::End();
    });

    /* start main loop */
    view.run();

    return EXIT_SUCCESS;
}
