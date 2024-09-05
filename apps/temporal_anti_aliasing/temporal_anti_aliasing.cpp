#include <cstdlib>

#include <viewer/viewer.h>
#include <viewer/asset/model.h>
#include <viewer/asset/obj_model.h>
#include <viewer/asset/shapes.h>
#include <viewer/opengl/shaderprogram.h>
#include <viewer/opengl/texture.h>
#include <viewer/opengl/framebuffer.h>

#include <glm/gtx/transform.hpp>

/* material and vertex definition */
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


/* halton sequence computation; with base 2 (x) and 3 (y) */
std::vector<glm::vec2> halton_sequence(int num)
{
    auto halton = [](int index, int base)
    {
        float result = 0.0f;
        float f = 1.0f / base;
        int i = index;

        while(i > 0)
        {
            result += f * (i % base);
            i = i / base;
            f = f / base;
        }

        return result;
    };

    std::vector<glm::vec2> samples(num);
    for(int i = 1; i <= num; i++)
    {
        samples[i - 1] = {halton(i, 2), halton(i, 3)};
    }

    return samples;
}

/********* Temporal Antialiasing Helper *********/
struct TAA
{
    std::vector<glm::vec2> jitter_noise = halton_sequence(8);
    int jitter_idx = 0;
    float alpha = 0.1;
    bool enabled = true;

    /* history buffer */
    struct History
    {
        glm::mat4 proj;
        opengl::handle<opengl::framebuffer> framebuffer;
        opengl::handle<opengl::texture> color;
    } history[2];
    int history_idx = 0;
};


int main(int argc, char** argv)
{
    /* initial window settings */
    viewer::window_settings settings;
    settings.title = "Temporal Anti-Aliasing";
    settings.width = 1280;
    settings.heigth = 720;

    /* construct viewer (creates window and context) */
    viewer view(settings);

    /* set camera position */
    auto& camera = view.camera();
    camera.position({3.25, 0.0, 0.0f});


    /* load model with specified vertex and material definitions (--> see asset/model.h for all possible member) */
    auto& context = view.context();
    auto model = asset::model_loader<vertex, material>::load_obj(context, "assets/sad_toaster/sad_toaster.obj");
    auto screen_quad = asset::shape<vertex>::create_screenquad(context);


    /* setup history color buffer */
    auto create_history_buffer = [&]()
    {
        auto tex_color = context.make_texture(opengl::texture_internal_type::rgb8, opengl::texture_format::rgb,
                                              opengl::texture_type::unsigned_byte_, view.window().size().x, view.window().size().y);

        tex_color->parameter(opengl::min_filter::linear);
        tex_color->parameter(opengl::mag_filter::linear);

        /* attach to gbuffer framebuffer */
        auto fb_gbuffer = context.make_framebuffer();
        fb_gbuffer->attach_color(0, tex_color);
        fb_gbuffer->draw_attachment(0);
        assert(fb_gbuffer->completed());

        return TAA::History{ glm::mat4(1.0), fb_gbuffer, tex_color };
    };

    /* temporal antialiasing helper */
    TAA taa;
    taa.history[0] = create_history_buffer();
    taa.history[1] = create_history_buffer();


    /* create pipeline framebuffer with color, velocity, and depth texture */
    auto tex_color = context.make_texture(opengl::texture_internal_type::rgb8, opengl::texture_format::rgb,
                                          opengl::texture_type::unsigned_byte_, view.window().size().x, view.window().size().y);
    auto tex_vel = context.make_texture(opengl::texture_internal_type::rgb16F, opengl::texture_format::rgb,
                                        opengl::texture_type::float_, view.window().size().x, view.window().size().y);
    auto tex_depth = context.make_texture(opengl::texture_internal_type::depth, opengl::texture_format::depth,
                                          opengl::texture_type::unsigned_int_, view.window().size().x, view.window().size().y);

    tex_color->parameter(opengl::min_filter::linear);
    tex_color->parameter(opengl::mag_filter::linear);

    tex_vel->parameter(opengl::min_filter::linear);
    tex_vel->parameter(opengl::mag_filter::linear);

    tex_depth->parameter(opengl::min_filter::linear);
    tex_depth->parameter(opengl::mag_filter::linear);

    /* attach to pipeline framebuffer */
    auto fb_pipeline = context.make_framebuffer();
    fb_pipeline->attach_color(0, tex_color);
    fb_pipeline->attach_color(1, tex_vel);
    fb_pipeline->attach_depth(tex_depth);
    fb_pipeline->draw_attachment(0, 1);
    assert(fb_pipeline->completed());


    /* create shader and compile */
    auto shader = context.make_shader();
    shader->load("temporal_anti_aliasing/shader/render.vert", opengl::shader_type::vertex);
    shader->load("temporal_anti_aliasing/shader/render.frag", opengl::shader_type::fragment);
    shader->link();

    auto shader_post = context.make_shader();
    shader_post->load("temporal_anti_aliasing/shader/post.vert", opengl::shader_type::vertex);
    shader_post->load("temporal_anti_aliasing/shader/post.frag", opengl::shader_type::fragment);
    shader_post->link();


    /* enable/disable OpenGL options */
    context.clear_color(1.0, 1.0, 1.0, 1.0);
    context.set(opengl::options::depth_test, true);


    /***************** install callbacks *****************/
    view.on_render([&](auto& window, float dt)
    {
        /* taa frame index */
        taa.jitter_idx = (taa.jitter_idx + 1) % taa.jitter_noise.size();

        int history_prev = taa.history_idx;
        taa.history_idx = (taa.history_idx + 1) % 2;

        /* store unjitter view-projection matrix */
        taa.history[taa.history_idx].proj = camera.projection() * camera.view();

        /*********** 1. render scene and store motion vectors ***********/
        fb_pipeline->bind();
        {
            context.clear(opengl::clear_options::color_depth);

            shader->bind();
            shader->uniform("uModel", glm::translate(glm::mat4(1.0), {0.0f, -1.0f, 0.0f}));
            shader->uniform("uProjViewCurr", taa.history[taa.history_idx].proj);
            shader->uniform("uProjViewPrev", taa.history[history_prev].proj);

            /* generate different samples by adding viewport sub-pixel offset (halton sequence) */
            auto jitter = (taa.jitter_noise[taa.jitter_idx] - glm::vec2{0.5, 0.5}) / glm::vec2(taa.history[history_prev].color->size());
            shader->uniform("uJitter", taa.enabled ? jitter : glm::vec2(0.0));

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
                    record.m_mesh.vao()->bind();
                    record.m_mesh.vao()->draw(record.m_offset, record.m_count, opengl::primitives::triangles);
                }
            }
        }
        fb_pipeline->unbind();

        /*********** 2. combine with temporal samples ***********/
        taa.history[taa.history_idx].framebuffer->bind();
        {
            context.clear(opengl::clear_options::color);

            shader_post->bind();
            shader_post->uniform("uColorPrev", 0);
            shader_post->uniform("uColorCurr", 1);
            shader_post->uniform("uVelocity", 2);
            shader_post->uniform("uAlpha", taa.enabled ? taa.alpha : 1.0f);

            taa.history[history_prev].color->bind(0);
            tex_color->bind(1);
            tex_vel->bind(2);

            screen_quad->vao()->draw(opengl::primitives::triangles);
        }
        taa.history[taa.history_idx].framebuffer->unbind();

        /* blit combined final image to default framebuffer */
        taa.history[taa.history_idx].framebuffer->blit_default(0, 0, window.size().x, window.size().y, opengl::blit_mask::color);
    });


    view.on_resize([&](auto& window, unsigned int width, unsigned int height)
    {
        taa.history[0].color->resize(width, height);
        taa.history[1].color->resize(width, height);

        tex_color->resize(width, height);
        tex_depth->resize(width, height);
        tex_vel->resize(width, height);
    });

    view.on_gui([&](auto& window, float dt)
    {
        /************** settings **************/
        ImGui::SetNextWindowPos({16, 16});
        ImGui::Begin("##Settings", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::TextColored({1.0, 1.0, 1.0, 1.0}, "FPS:       %4.2f        ", view.frameclock().fps());
            ImGui::Dummy({0.0, 16.0});

            ImGui::TextColored({0.7, 0.7, 0.7, 1.0}, "TAA:");
            ImGui::Checkbox("enabled", &taa.enabled);
            ImGui::SliderFloat("blend factor", &taa.alpha, 0.0, 1.0);

            ImGui::Image(taa.history[taa.history_idx].color.get(), {256, 256}, {0.4, 0.5}, {0.5, 0.4});
        }
        ImGui::End();
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

    /* start main loop */
    view.run();

    return EXIT_SUCCESS;
}
