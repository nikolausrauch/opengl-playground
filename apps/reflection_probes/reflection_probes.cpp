#include "glm/gtc/type_ptr.hpp"
#include <cstdlib>

#include <viewer/viewer.h>
#include <viewer/asset/model.h>
#include <viewer/asset/obj_model.h>
#include <viewer/asset/shapes.h>
#include <viewer/asset/texture.h>

#include <viewer/opengl/shaderprogram.h>
#include <viewer/opengl/texture.h>
#include <viewer/opengl/texturecube.h>
#include <viewer/opengl/framebuffer.h>

#include <glm/gtx/transform.hpp>

/************** materials and vertex definition **************/
struct material_scene
{
    opengl::handle<opengl::texture> map_diffuse;
};

struct material_water
{
    opengl::handle<opengl::texture> map_diffuse;
    opengl::handle<opengl::texture> map_normal;
};

struct vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
};

template <>
struct opengl::layout<vertex>
{
    static constexpr attr_info value[] = {
                                          {opengl::type::float_, 3, opengl::buffer_mapping::cast, offsetof(vertex, position)},
                                          {opengl::type::float_, 3, opengl::buffer_mapping::cast, offsetof(vertex, normal)},
                                          {opengl::type::float_, 2, opengl::buffer_mapping::cast, offsetof(vertex, texcoord)}};
};


/************** reflection propes and settings **************/
struct reflection_settings
{
    bool reflections = true;
    bool parallax = true;
    bool normal_map = true;
    bool render_volume = false;
    int selected = 0;
};

struct reflection_prope
{
    opengl::handle<opengl::framebuffer> framebuffer;
    opengl::handle<opengl::texture_cube> tex_color;
    opengl::handle<opengl::texture_cube> tex_depth;

    glm::vec3 position;
    glm::vec3 extends;
};

/* construct framebuffer with cubemap attachments */
reflection_prope construct_prope(opengl::context& gl_context, const glm::uvec2& resolution, const glm::vec3& pos = {0.0, 0.0, 0.0}, const glm::vec3& extends = {1.0, 1.0, 1.0})
{
    auto tex_cube_color = gl_context.make_texture_cube(opengl::texture_internal_type::rgba8, opengl::texture_format::rgba, opengl::texture_type::unsigned_byte_, 1024, 1024);
    auto tex_cube_depth = gl_context.make_texture_cube(opengl::texture_internal_type::depth, opengl::texture_format::depth, opengl::texture_type::float_, 1024, 1024);

    tex_cube_color->parameter(opengl::min_filter::linear);
    tex_cube_color->parameter(opengl::mag_filter::linear);

    auto fb_cube = gl_context.make_framebuffer();
    fb_cube->attach_color(0, tex_cube_color);
    fb_cube->attach_depth(tex_cube_depth);
    fb_cube->draw_attachment(0);

    return reflection_prope{ fb_cube, tex_cube_color, tex_cube_depth, pos, extends };
}

/* render scene from prope viewpoint to cubemap */
void render_prope(reflection_prope& refl_prope, opengl::context& gl_context, opengl::handle<opengl::shader_program>& shader_prope, const std::function<void(opengl::handle<opengl::shader_program>&, bool)>& render_func)
{
    refl_prope.framebuffer->bind();
    {
        auto size = refl_prope.tex_color->size(opengl::cube_face::positive_x);
        auto viewport = gl_context.viewport(0, 0, size.x, size.y);
        gl_context.clear(opengl::clear_options::color_depth);

        auto prope_pos = refl_prope.position;

        /* setup view matrices for each cube face */
        std::array<glm::mat4, 6> views =
            {
                glm::lookAt(prope_pos, prope_pos + glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),  // +X
                glm::lookAt(prope_pos, prope_pos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),  // -X
                glm::lookAt(prope_pos, prope_pos + glm::vec3(0.0f,  1.0f, 0.0f), glm::vec3(0.0f,  0.0f, 1.0f)),  // +Y
                glm::lookAt(prope_pos, prope_pos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f)), // -Y
                glm::lookAt(prope_pos, prope_pos + glm::vec3(0.0f, 0.0f,  1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),  // +Z
                glm::lookAt(prope_pos, prope_pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))   // -Z
            };

        /* currently assume all faces have the same size (aabb with equal axis sizes) */
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);

        /* render scene to cubemap */
        shader_prope->bind();
        shader_prope->uniform("uModel", glm::translate(glm::mat4(1.0), {0.0f, -1.0f, 0.0f}));
        shader_prope->uniform("projection", projection);
        shader_prope->uniform("captureViews[0]", views);

        render_func(shader_prope, false);
        render_func(shader_prope, true);

        gl_context.viewport(viewport);
    }
    refl_prope.framebuffer->unbind();
}


int main(int argc, char** argv)
{
    /* initial window settings */
    viewer::window_settings settings;
    settings.title = "Reflection Probes";
    settings.width = 1280;
    settings.heigth = 720;

    /* construct viewer (creates window and context) */
    viewer view(settings);

    auto& camera = view.camera();
    camera.position({9.0, 1.0, 17.0});
    camera.look_at({5.0, 0.0, 0.0});
    camera.clip_planes(0.1, 100.0);


    /* load model (transparent and reflective textures are handled manually!) */
    auto& context = view.context();
    auto model = asset::model_loader<vertex, material_scene>::load_obj(context, "assets/wanderer/wanderer.obj");
    auto water = asset::model_loader<vertex, material_water>::load_obj(context, "assets/wanderer/water.obj");
    auto mesh_cube = asset::shape<vertex>::create_unitcube(context);


    /* reflection propes and settings */
    reflection_settings refl_settings;
    std::vector<reflection_prope> refl_propes;
    refl_propes.reserve(8);
    refl_propes.emplace_back(construct_prope(context, {128, 128}, {5.5, 0.0, -10.0}, {14.9, 14.9, 14.9}));
    refl_propes.emplace_back(construct_prope(context, {128, 128}, {8.9, 0.0, 11.7}, {9.0, 9.0, 9.0}));
    refl_propes.emplace_back(construct_prope(context, {128, 128}, {-16.0, 0.0, -9.1}, {13.3, 13.3, 13.3}));
    refl_propes.emplace_back(construct_prope(context, {128, 128}, {-3.1, 0.0,  13.9}, {8.7, 8.7, 8.7}));


    /* create shader and compile */
    auto shader = context.make_shader();
    shader->load("reflection_probes/shader/scene.vert", opengl::shader_type::vertex);
    shader->load("reflection_probes/shader/scene.frag", opengl::shader_type::fragment);
    shader->link();

    auto shader_water = context.make_shader();
    shader_water->load("reflection_probes/shader/scene.vert", opengl::shader_type::vertex);
    shader_water->load("reflection_probes/shader/water.frag", opengl::shader_type::fragment);
    shader_water->link();

    auto shader_debug = context.make_shader();
    shader_debug->load("reflection_probes/shader/scene.vert", opengl::shader_type::vertex);
    shader_debug->load("reflection_probes/shader/debug_prope.frag", opengl::shader_type::fragment);
    shader_debug->link();

    auto shader_probe = context.make_shader();
    shader_probe->load("reflection_probes/shader/refl_prope.vert", opengl::shader_type::vertex);
    shader_probe->load("reflection_probes/shader/refl_prope.geom", opengl::shader_type::geometry);
    shader_probe->load("reflection_probes/shader/scene.frag", opengl::shader_type::fragment);
    shader_probe->link();


    /* enable/disable OpenGL options */
    context.set(opengl::options::cube_map_seamless, true);
    context.set(opengl::options::depth_test, true);
    context.set(opengl::options::blend, true);
    context.set(opengl::blend_func_factor_alpha::src_alpha, opengl::blend_func_factor_alpha::one_minus_src_alpha);
    context.clear_color(0.0, 0.0, 0.0, 1.0);


    /* render scene func */
    auto render_scene = [model](auto& shader, bool transparent_only = false)
    {
        /* iterate over materials */
        for(auto& [_, mat_group] : model->material_groups())
        {
            /* !! manual transparancy handling; TODO: should be handled in model by material !! */
            if(!transparent_only && mat_group.name() == "z_Lamp_1") { continue; }
            if(transparent_only && mat_group.name() != "z_Lamp_1") { continue; }

            /* set material uniforms */
            auto& mat = mat_group.material();
            shader->uniform("uMapDiffuse", 1);
            mat.map_diffuse->bind(1);

            for(const auto& record : mat_group.records())
            {
                record.m_mesh.vao()->draw(record.m_offset, record.m_count, opengl::primitives::triangles);
            }
        }
    };

    /* precompute prope cube maps */
    for(auto& prope : refl_propes)
    {
        render_prope(prope, context, shader_probe, render_scene);
    }


    /***************** install callbacks *****************/
    view.on_render([&](auto& window, float dt)
    {
        /********** render water **********/
        {
            shader_water->bind();
            shader_water->uniform("uModel", glm::translate(glm::mat4(1.0), {0.0f, -1.0f, 0.0f}));
            shader_water->uniform("uView", camera.view());
            shader_water->uniform("uProj", camera.projection());
            shader_water->uniform("uViewPos", camera.position());
            shader_water->uniform("uReflections", refl_settings.reflections);
            shader_water->uniform("uNormalMap", refl_settings.normal_map);
            shader_water->uniform("uParallax", refl_settings.parallax);

            /* set prope parameter */
            for(unsigned int i = 0; i < refl_propes.size(); i++)
            {
                auto& prope = refl_propes[i];
                std::string str_access = std::string("uReflPropes[") + std::to_string(i) + "].";
                shader_water->uniform(str_access + "pos", prope.position);
                shader_water->uniform(str_access + "extends", prope.extends);
                shader_water->uniform(str_access + "map", static_cast<int>(3 + i));
                prope.tex_color->bind(3 + i);
            }
            shader_water->uniform("uNumPropes", static_cast<int>(refl_propes.size()));

            /* water material */
            auto& water_group =  water->material_groups().at("Water");
            auto& water_mat = water_group.material();

            shader_water->uniform("uMapDiffuse", 1);
            water_mat.map_diffuse->bind(1);

            shader_water->uniform("uNormal", 2);
            water_mat.map_normal->bind(2);

            for(const auto& record : water_group.records())
            {
                record.m_mesh.vao()->draw(record.m_offset, record.m_count, opengl::primitives::triangles);
            }
        }

        /********** render scene **********/
        {
            shader->bind();
            shader->uniform("uModel", glm::translate(glm::mat4(1.0), {0.0f, -1.0f, 0.0f}));
            shader->uniform("uView", camera.view());
            shader->uniform("uProj", camera.projection());
            render_scene(shader, false);
            render_scene(shader, true);
        }


        /********** debug render prope volume **********/
        if(refl_settings.render_volume)
        {
            shader_debug->bind();
            shader_debug->uniform("uView", camera.view());
            shader_debug->uniform("uProj", camera.projection());

            for(unsigned int i = 0; i < refl_propes.size(); i++)
            {
                auto& propes = refl_propes[i];
                shader_debug->uniform("uSelected", ( refl_settings.selected == static_cast<int>(i) ));
                shader_debug->uniform("uModel", glm::translate(propes.position) * glm::scale(2.0f * propes.extends));
                mesh_cube->vao()->draw(opengl::primitives::triangles);
            }
        }

    });


    view.on_key([&](auto& window, auto key, bool pressed)
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

            /* reflection  probe controls */
            ImGui::TextColored({1.0, 1.0, 0, 1.0}, "Reflection Probes: ");
            ImGui::PushID("propes");
            if(ImGui::SliderInt("Prope ID", &refl_settings.selected, 0, refl_propes.size() - 1)) {}

            if(ImGui::DragFloat3("position",  &refl_propes[refl_settings.selected].position[0], 0.1f))
            {
                render_prope(refl_propes[refl_settings.selected], context, shader_probe, render_scene);
            }

            if(ImGui::DragFloat("extends",  &refl_propes[refl_settings.selected].extends[0], 0.1f))
            {
                refl_propes[refl_settings.selected].extends = {refl_propes[refl_settings.selected].extends.x, refl_propes[refl_settings.selected].extends.x, refl_propes[refl_settings.selected].extends.x};
                render_prope(refl_propes[refl_settings.selected], context, shader_probe, render_scene);
            }
            ImGui::PopID();

            /* global render settings */
            ImGui::Checkbox("reflections", &refl_settings.reflections);
            ImGui::Checkbox("parallax correction", &refl_settings.parallax);
            ImGui::Checkbox("water normal map", &refl_settings.normal_map);
            ImGui::Checkbox("render probe volume", &refl_settings.render_volume);
        }
        ImGui::End();

    });

    /* start main loop */
    view.run();

    return EXIT_SUCCESS;
}
