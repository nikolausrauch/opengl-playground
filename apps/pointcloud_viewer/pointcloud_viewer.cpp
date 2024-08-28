#include <cstdlib>

#include <viewer/viewer.h>
#include <viewer/asset/model.h>
#include <viewer/asset/pointcloud.h>
#include <viewer/opengl/shaderprogram.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <iostream>

struct vertex
{
    glm::vec3 position;
    glm::vec3 color;
};

/* vertexbuffer layout definition for OpenGL */
template <>
struct opengl::layout<vertex>
{
    static constexpr attr_info value[] = {
        {opengl::type::float_, 3, opengl::buffer_mapping::cast, offsetof(vertex, position)},
        {opengl::type::float_, 3, opengl::buffer_mapping::cast, offsetof(vertex, color)},
        };
};

struct pointcloud_settings
{
    /* light source */
    glm::vec3 direction = glm::vec3(glm::eulerAngleYXZ(glm::quarter_pi<float>(), glm::quarter_pi<float>(), glm::quarter_pi<float>()) * glm::vec4(0, -1, 0, 0));

    glm::vec3 ambient = {0.3, 0.3, 0.3};
    glm::vec3 color = {1.0, 1.0, 1.0};

    /* point radius */
    float radius = 0.0036;

    /* material */
    float shininess = 32.0;
    glm::vec3 specular = {0.0, 0.0, 0.0};

    /* light control */
    glm::vec2 angles = {glm::quarter_pi<float>(), glm::quarter_pi<float>()};
};


int main(int argc, char** argv)
{
    /* initial window settings */
    viewer::window_settings settings;
    settings.title = "Pointcloud Viewer";
    settings.width = 1280;
    settings.heigth = 720;

    /* construct viewer (creates window and context) */
    viewer view(settings);

    /* set camera position */
    auto& camera = view.camera();
    camera.position({0.0075, 0.35, 0.65});
    camera.clip_planes(0.1, 50.0);

    auto& context = view.context();

    /* load pointcloud from ply file */
    auto cloud = asset::pointcloud_loader<vertex>::load_ply(context, "assets/pc_porsche/porsche.ply");
    if(!cloud)
    {
        std::cerr << "Failure while reading pointcloud file!" << std::endl;
        return EXIT_FAILURE;
    }

    /* create shader and compile */
    auto shader = context.make_shader();
    shader->load("pointcloud_viewer/shader/billboard.vert", opengl::shader_type::vertex);
    shader->load("pointcloud_viewer/shader/billboard.geom", opengl::shader_type::geometry);
    shader->load("pointcloud_viewer/shader/blinn_phong.frag", opengl::shader_type::fragment);
    shader->link();

    /* enable/disable OpenGL options */
    context.set(opengl::options::depth_test, true);
    context.clear_color(1.0, 1.0, 1.0, 1.0);

    /* light, material, and point cloud settings */
    pointcloud_settings pc_settings;


    /***************** install callbacks *****************/
    view.on_render([&](auto& window, float dt)
    {
        shader->bind();

        auto model = glm::mat4(1.0);
        shader->uniform("uModel", model);
        shader->uniform("uView", camera.view());
        shader->uniform("uProj", camera.projection());
        shader->uniform("uRadius", pc_settings.radius);

        shader->uniform("uLight.direction", glm::mat3(camera.view()) * glm::normalize(pc_settings.direction) );
        shader->uniform("uLight.ambient", pc_settings.ambient);
        shader->uniform("uLight.color", pc_settings.color);

        shader->uniform("uMaterial.specular", pc_settings.specular);
        shader->uniform("uMaterial.shininess", pc_settings.shininess);

        cloud->vao()->draw(opengl::primitives::points);
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
        ImGui::SetNextWindowPos({16, 16});
        ImGui::Begin("##Settings", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::TextColored({1.0, 1.0, 1.0, 1.0}, "FPS:       %4.2f        ", view.frameclock().fps());


            ImGui::Dummy({0.0, 16.0});


            ImGui::TextColored({1.0, 1.0, 0, 1.0}, "Light: ");
            ImGui::PushID("light");
            if(ImGui::DragFloat2("direction",  &pc_settings.angles[0], 0.01, -2.0f*glm::pi<float>(), 2.0f*glm::pi<float>()))
            {
                glm::vec3 dir =
                    {
                        glm::sin(pc_settings.angles.x) * glm::sin(pc_settings.angles.y),
                        glm::cos(pc_settings.angles.y),
                        glm::cos(pc_settings.angles.x) * glm::sin(pc_settings.angles.y)
                    };

                pc_settings.direction = glm::normalize(-dir);
            }
            ImGui::ColorEdit3("ambient",  &pc_settings.ambient[0]);
            ImGui::ColorEdit3("color",  &pc_settings.color[0]);
            ImGui::PopID();


            ImGui::Dummy({0.0, 16.0});


            ImGui::TextColored({1.0, 1.0, 0, 1.0}, "Point Cloud: ");
            ImGui::PushID("pc");
            ImGui::DragFloat("radius", &pc_settings.radius, 0.0001, 0.00001, 0.1, "%.5f");
            ImGui::DragFloat("shininess",  &pc_settings.shininess, 1.0, 0.0, 1024.0);
            ImGui::ColorEdit3("specular",  &pc_settings.specular[0]);
            ImGui::PopID();
        }
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(16, window.size().y - 50));
        ImGui::Begin("##Desc", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
        {
            ImGui::TextColored({0.5, 0.5, 0, 1.0},   "[Mouse Left Button] "); ImGui::SameLine(); ImGui::TextColored({0.2, 0.2, 0.2, 0.9}, " Camera Control");
        }
        ImGui::End();
    });

    /* start main loop */
    view.run();

    return EXIT_SUCCESS;
}
