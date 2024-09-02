#include <cstdlib>

#include <viewer/viewer.h>
#include <viewer/asset/model.h>
#include <viewer/asset/obj_model.h>
#include <viewer/asset/shapes.h>
#include <viewer/opengl/shaderprogram.h>
#include <viewer/opengl/texture.h>
#include <viewer/opengl/framebuffer.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/euler_angles.hpp>


struct material
{
    float shininess = 0.0;
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

/* Deferred Rendering Control */
struct deferred_rendering
{
    bool dir_light_pass = true;
    bool spot_light_pass = true;
    bool light_volume_debug = false;
    float distance_scale = 0.03f;
};

/* Directional Light Source */
struct light_dir
{
    glm::vec3 angles = glm::vec3(glm::quarter_pi<float>(), glm::quarter_pi<float>(), glm::quarter_pi<float>());
    glm::vec3 direction = -glm::normalize(  glm::vec3(glm::sin(angles.x) * glm::sin(angles.y),
                                                    glm::cos(angles.y),
                                                    glm::cos(angles.x) * glm::sin(angles.y)));

    glm::vec3 ambient = {0.1, 0.1, 0.1};
    glm::vec3 color = {0.0, 0.0, 0.2};
};

/* Spot Light Source (Layout to match std430 in shader) */
struct light_spot
{
    alignas(8) glm::vec3 position {0.0, 0.0, 0.0};;
    alignas(8) glm::vec3 direction = {0.0, -1.0, 0.0};

    alignas(8) glm::vec3 color = {1.0, 1.0, 0.6};

    alignas(4) float inner = glm::pi<float>() / 10.0f;
    alignas(4) float outer = glm::pi<float>() / 5.0f;

    alignas(4) float constant = 1.0;
    alignas(4) float linear = 0.09;
    alignas(4) float quadratic = 0.032;
};

glm::mat4 spot_transform(const light_spot& spot, float distance_scale)
{
    float ill_max = std::max<float>({ spot.color.r, spot.color.g, spot.color.b });
    float distance_max = distance_scale * (-spot.linear + std::sqrt(spot.linear * spot.linear - 4.0f * spot.quadratic * (spot.constant - (256.0f / 5.0f) * ill_max))) / (2.0f * spot.quadratic);

    glm::vec3 default_dir = {0.0, -1.0, 0.0};
    auto rotate = glm::mat4(1.0);
    if( !(default_dir == spot.direction) )
    {
        auto normal = glm::cross(spot.direction, default_dir);
        float angle = glm::acos(glm::dot(spot.direction, default_dir));
        rotate = glm::rotate(rotate, angle, normal);
    }

    float extends = glm::tan(spot.outer) * distance_max;
    auto scale = glm::scale(glm::mat4(1.0), { extends, distance_max, extends });
    auto translate = glm::translate(glm::mat4(1.0), spot.position);

    return translate * rotate * scale;
}

int main(int argc, char** argv)
{
    /* initial window settings */
    viewer::window_settings settings;
    settings.title = "Deferred Rendering";
    settings.width = 1280;
    settings.heigth = 720;

    /* construct viewer (creates window and context) */
    viewer view(settings);
    auto& context = view.context();

    auto& camera = view.camera();
    camera.position({-5.0, 3.0, -6.0});

    deferred_rendering render_control;

    /* directional and spot lights */
    light_dir lightdir;

    auto model_matrix = glm::translate(glm::scale(glm::vec3{1.0f, 1.0f, 1.0f} * 0.05f), {0.0f, 2.0f, 0.0f});
    std::vector<light_spot> light_spots({
    { .position = glm::vec3(model_matrix * glm::vec4{15.0, 14.5, 5.2, 1.0})  },
    { .position = glm::vec3(model_matrix * glm::vec4{40.0, 14.5, 5.2, 1.0})  },
    { .position = glm::vec3(model_matrix * glm::vec4{51.0, 14.5, 30.0, 1.0}) },
    { .position = glm::vec3(model_matrix * glm::vec4{51.0, 14.5,-25.0, 1.0}) },
    { .position = glm::vec3(model_matrix * glm::vec4{14.5, 14.5, -1.0, 1.0}) },
    { .position = glm::vec3(model_matrix * glm::vec4{39.5, 14.5, -1.0, 1.0}) },
    { .position = glm::vec3(model_matrix * glm::vec4{-2.3, 14.5, 15.5, 1.0}) },
    { .position = glm::vec3(model_matrix * glm::vec4{-2.3, 14.5, 43.0, 1.0}) },
    { .position = glm::vec3(model_matrix * glm::vec4{ 16.0, 14.5, 52.0, 1.0}) },
    { .position = glm::vec3(model_matrix * glm::vec4{-49.0, 14.5, 43.0, 1.0}) },
    { .position = glm::vec3(model_matrix * glm::vec4{-49.0, 14.5, 15.5, 1.0}) },
    { .position = glm::vec3(model_matrix * glm::vec4{-49.0, 15.5,-25.5, 1.0}) },
    { .position = glm::vec3(model_matrix * glm::vec4{-15.5, 15.5,-40.5, 1.0}) },
    { .position = glm::vec3(model_matrix * glm::vec4{-15.5, 15.5, -9.5, 1.0}) },
    { .position = glm::vec3(model_matrix * glm::vec4{ 15.5, 14.5,-48.5, 1.0}) },
    { .position = glm::vec3(model_matrix * glm::vec4{ 39.5, 14.5,-48.5, 1.0}) }
    });


    /* load obj model */
    auto model = asset::model_loader<vertex, material>::load_obj(context, "assets/small_city/small_city.obj");
    auto screen_quad = asset::shape<vertex>::create_screenquad(context);
    auto mesh_spots = asset::shape<vertex>::create_pyramid(context);
    auto buffer_spots = context.make_buffer<light_spot>(opengl::buffer_target::shader_storage, light_spots, opengl::buffer_usage::dynamic_draw);

    /* load and compile shaders */
    auto shader_gpass = context.make_shader();
    shader_gpass->load("deferred_rendering/shader/gpass.vert", opengl::shader_type::vertex);
    shader_gpass->load("deferred_rendering/shader/gpass.frag", opengl::shader_type::fragment);
    shader_gpass->link();

    auto shader_lightdir = context.make_shader();
    shader_lightdir->load("deferred_rendering/shader/light_dir.vert", opengl::shader_type::vertex);
    shader_lightdir->load("deferred_rendering/shader/light_dir.frag", opengl::shader_type::fragment);
    shader_lightdir->link();

    auto shader_lightspots = context.make_shader();
    shader_lightspots->load("deferred_rendering/shader/light_spots.vert", opengl::shader_type::vertex);
    shader_lightspots->load("deferred_rendering/shader/light_spots.frag", opengl::shader_type::fragment);
    shader_lightspots->link();

    auto shader_debug = context.make_shader();
    shader_debug->load("deferred_rendering/shader/debug_light.vert", opengl::shader_type::vertex);
    shader_debug->load("deferred_rendering/shader/debug_light.frag", opengl::shader_type::fragment);
    shader_debug->link();


    /* setup gbuffer textures */
    auto tex_pos = context.make_texture(opengl::texture_internal_type::rgb16F, opengl::texture_format::rgb,
                                        opengl::texture_type::float_, view.window().size().x, view.window().size().y);
    auto tex_normal = context.make_texture(opengl::texture_internal_type::rgb16F, opengl::texture_format::rgb,
                                           opengl::texture_type::float_, view.window().size().x, view.window().size().y);
    auto tex_material = context.make_texture(opengl::texture_internal_type::rgba8, opengl::texture_format::rgba,
                                             opengl::texture_type::unsigned_byte_, view.window().size().x, view.window().size().y);
    auto tex_depth = context.make_texture(opengl::texture_internal_type::depth, opengl::texture_format::depth,
                                          opengl::texture_type::unsigned_int_, view.window().size().x, view.window().size().y);

    tex_pos->parameter(opengl::min_filter::nearest);
    tex_pos->parameter(opengl::mag_filter::nearest);

    tex_normal->parameter(opengl::min_filter::nearest);
    tex_normal->parameter(opengl::mag_filter::nearest);

    tex_material->parameter(opengl::min_filter::nearest);
    tex_material->parameter(opengl::mag_filter::nearest);

    tex_depth->parameter(opengl::min_filter::nearest);
    tex_depth->parameter(opengl::mag_filter::nearest);


    /* attach to gbuffer framebuffer */
    auto fb_gbuffer = context.make_framebuffer();
    fb_gbuffer->attach_color(0, tex_pos);
    fb_gbuffer->attach_color(1, tex_normal);
    fb_gbuffer->attach_color(2, tex_material);
    fb_gbuffer->attach_depth(tex_depth);
    fb_gbuffer->draw_attachment(0, 1, 2);
    assert(fb_gbuffer->completed());


    /* set context state */
    context.set(opengl::options::depth_test, true);
    context.set(opengl::options::cull_face, true);
    context.cull(opengl::polygon_face::back);


    /***************** install callbacks *****************/
    view.on_render([&](auto& window, float dt)
    {
        /************************** 1. Geometry pass *****************************/
        fb_gbuffer->bind();
        {
            context.viewport(0, 0, window.size().x, window.size().y);
            context.clear_color(0, 0, 0,  1);
            context.clear(opengl::clear_options::color_depth);

            shader_gpass->bind();
            shader_gpass->uniform("uModel", model_matrix);
            shader_gpass->uniform("uView", camera.view());
            shader_gpass->uniform("uProj", camera.projection());
            shader_gpass->uniform("uMaterial.map_diffuse", 0);

            /* iterate over materials */
            for(auto& [_, mat_group] : model->material_groups())
            {
                /* set material uniforms */
                auto& mat = mat_group.material();
                shader_gpass->uniform("uMaterial.shininess", mat.shininess);
                mat.map_diffuse->bind(0);

                /* iterate over mesh and render faces with this material */
                for(const auto& record : mat_group.records())
                {
                    record.m_mesh.vao()->bind();
                    record.m_mesh.vao()->draw(record.m_offset, record.m_count, opengl::primitives::triangles);
                }
            }
        }
        fb_gbuffer->unbind();


        /************************** 2. Directional light pass *****************************/
        if(render_control.dir_light_pass)
        {
            context.set(opengl::options::cull_face, false);

            shader_lightdir->bind();
            shader_lightdir->uniform("gBuffer.pos", 0);
            shader_lightdir->uniform("gBuffer.normal", 1);
            shader_lightdir->uniform("gBuffer.material", 2);

            shader_lightdir->uniform("uViewPos", camera.position());
            shader_lightdir->uniform("uViewSize", glm::vec2(window.size()));

            shader_lightdir->uniform("uLight.direction", lightdir.direction);
            shader_lightdir->uniform("uLight.ambient", lightdir.ambient);
            shader_lightdir->uniform("uLight.color", lightdir.color);

            tex_pos->bind(0);
            tex_normal->bind(1);
            tex_material->bind(2);

            screen_quad->vao()->draw(opengl::primitives::triangles);

            context.set(opengl::options::cull_face, true);
        }

        fb_gbuffer->blit_default(0, 0, window.size().x, window.size().y, opengl::blit_mask::depth);


        /************************** 3. Spot light pass *****************************/
        if(render_control.spot_light_pass)
        {
            auto blend = context.set(opengl::options::blend, true);
            context.set(opengl::blend_func_factor_alpha::src_alpha, opengl::blend_func_factor_alpha::one);
            context.set(opengl::options::depth_test, true);
            auto mask = context.depth_mask(false);

            shader_lightspots->bind();
            shader_lightspots->uniform("uModel", model_matrix);
            shader_lightspots->uniform("uView", camera.view());
            shader_lightspots->uniform("uProj", camera.projection());
            shader_lightspots->uniform("uViewPos", camera.position());
            shader_lightspots->uniform("uViewSize", glm::vec2(window.size()));

            shader_lightspots->uniform("gBuffer.pos", 0);
            shader_lightspots->uniform("gBuffer.normal", 1);
            shader_lightspots->uniform("gBuffer.material", 2);


            tex_pos->bind(0);
            tex_normal->bind(1);
            tex_material->bind(2);

            buffer_spots->bind_base(0);

            for(unsigned int i = 0; i < light_spots.size(); i++)
            {
                auto& light = light_spots[i];
                shader_lightspots->uniform("uLightIndex", static_cast<int>(i));
                shader_lightspots->uniform("uModel", spot_transform(light, render_control.distance_scale));

                mesh_spots->vao()->draw(opengl::primitives::triangles);
            }

            context.set(opengl::options::blend, blend);
            context.depth_mask(mask);
        }

        if(render_control.light_volume_debug)
        {
            auto mode = context.set(opengl::polygon_mode::line);

            shader_debug->bind();
            shader_debug->uniform("uModel", model_matrix);
            shader_debug->uniform("uView", camera.view());
            shader_debug->uniform("uProj", camera.projection());

            for(unsigned int i = 0; i < light_spots.size(); i++)
            {
                auto& light = light_spots[i];
                shader_debug->uniform("uModel", spot_transform(light, render_control.distance_scale));
                mesh_spots->vao()->draw(opengl::primitives::triangles);
            }

            context.set(mode);
        }

    });

    view.on_resize([&](auto& window, unsigned int width, unsigned int height)
    {
        tex_pos->resize(width, height);
        tex_material->resize(width, height);
        tex_normal->resize(width, height);
        tex_depth->resize(width, height);
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
            if(ImGui::DragFloat2("direction",  &lightdir.angles[0], 0.01,
                                  -2.0f*glm::pi<float>(),
                                   2.0f*glm::pi<float>()))
            {
                glm::vec3 dir =
                    {
                        glm::sin(lightdir.angles.x) * glm::sin(lightdir.angles.y),
                        glm::cos(lightdir.angles.y),
                        glm::cos(lightdir.angles.x) * glm::sin(lightdir.angles.y)
                    };

                lightdir.direction = glm::normalize(-dir);
            }
            ImGui::ColorEdit3("ambient",  &lightdir.ambient[0]);
            ImGui::ColorEdit3("color",  &lightdir.color[0]);
            ImGui::PopID();

            if(!light_spots.empty())
            {
                bool changed = false;

                ImGui::TextColored({1.0, 1.0, 0, 1.0}, "Spot Lights: ");
                ImGui::PushID("spotlight");
                if(ImGui::DragFloat("inner cutoff",  &light_spots[0].inner, 0.01, 0.0, light_spots[0].outer))
                {
                    std::for_each(light_spots.begin(), light_spots.end(), [&](auto& light){ light.inner = light_spots[0].inner; });
                    changed = true;
                }
                if(ImGui::DragFloat("outer cutoff",  &light_spots[0].outer, 0.01, light_spots[0].inner, glm::pi<float>()))
                {
                    std::for_each(light_spots.begin(), light_spots.end(), [&](auto& light){ light.outer = light_spots[0].outer; });
                    changed = true;
                }
                if(ImGui::ColorEdit3("color",  &light_spots[0].color[0]))
                {
                    std::for_each(light_spots.begin(), light_spots.end(), [&](auto& light){ light.color = light_spots[0].color; });
                    changed = true;
                }
                ImGui::PopID();

                ImGui::DragFloat("light volume scale", &render_control.distance_scale, 0.01f, 0.0f, 1.0f);

                if(changed)
                {
                    buffer_spots->data(light_spots);
                }
            }

            ImGui::Checkbox("directlight pass", &render_control.dir_light_pass);
            ImGui::Checkbox("spotlight pass", &render_control.spot_light_pass);
            ImGui::Checkbox("lightvolume debug", &render_control.light_volume_debug);

        }
        ImGui::End();

        /************** render shadow map **************/
        ImGui::SetNextWindowPos({16.0f, window.size().y - 256.0f - 32.0f});
        ImGui::Begin("##GBuffer", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::Image(tex_pos.get(), {128, 128}, {0, 1}, {1, 0});
            ImGui::SameLine();
            ImGui::Image(tex_normal.get(), {128, 128}, {0, 1}, {1, 0});

            ImGui::Image(tex_material.get(), {128, 128}, {0, 1}, {1, 0});
            ImGui::SameLine();
            ImGui::Image(tex_depth.get(), {128, 128}, {0, 1}, {1, 0});
        }
        ImGui::End();
    });

    /* start main loop */
    view.run();

    return EXIT_SUCCESS;
}
