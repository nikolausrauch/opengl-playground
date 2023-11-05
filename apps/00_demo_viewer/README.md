<h1 align="center">Viewer</h1>

## Window, Input and GUI 

The [viewer](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/viewer.h) class handles window (gl context) creation, input events, and callback registration.  
On construction some initial settings (window title, size, vertical synchronization) can be provided.
The option **hdpi** scales window size and *ImGui* font size (this can't be changed during runtime).

```C++
viewer::window_settings settings;
settings.title = "Colored Cube";
settings.width = 1280;
settings.heigth = 720;
settings.vsync = true;
settings.hdpi = false;

viewer view(settings);
```
Rendering, input/event handling, *ImGui* and *ImPlot* commands are handled via callback functions.
These are registered at the [viewer](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/viewer.h) instance.
(**Do not** replace the callback while it is executed!)

```C++
/***************** install callbacks *****************/
view.on_update([](auto& window, float dt)
{
    /* called once per frame before render call */
});

view.on_render([&](auto& window, float dt)
{
    /* called once per frame (framebuffer is already cleared) */
});

view.on_gui([](auto& window, float dt)
{
    /* submit imgui / implot calls from here (new frame, end frame called in viewer) */
    ImGui::ShowDemoWindow();
    ImPlot::ShowDemoWindow();
});

view.on_mouse_button([](auto& window, auto button, auto pos, bool pressed)
{
    /* called on mouse button events */
});

view.on_resize([](auto& window, unsigned int width, unsigned int height)
{
    /* called on window resize */
});

view.on_key([](auto& window, auto key, bool pressed)
{
    /* called on key event */

    /* exit on escape */
    if(key == core::keyboard::key::escape && pressed)
    {
        window.close();
    }
});
```
To start the *main loop*, which in turn executes the previously defined callbacks, call the method **run()**.
The program will return from this function when the window is closed.
```C++
view.run();
```
---

## OpenGL Context (Buffer and Shaders)
The OpenGL context is created on [viewer](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/viewer.h) construction and is destroyed on destruction - OpenGL calls are only allowed in between.
For ease of use I wrapped some of the most commonly used OpenGL functionalities (not feature complete), which is accessed through an [opengl::context](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/opengl/context.h) instance.   
You can still use raw opengl functions but be aware that I track binding identifiers and state options in the context to avoid redundant OpenGL calls (so be sure to reset the state!).
```C++
auto& context = view.context();
```
For example, creating a [shader program](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/opengl/shaderprogram.h) with a vertex and fragment shader:
```C++
const char *vertex_shader = "
    #version 330\n
    layout(location = 0) in vec3 aPosition;
    layout(location = 1) in vec4 aColor;

    out vec4 tColor;

    void main(void)
    {
        gl_Position = vec4(aPosition, 1.0);
        tColor = aColor;
    });"

const char *frag_shader = "
    #version 330\n
    in vec4 tColor;

    out vec4 fragColor;

    void main(void)
    {
        fragColor = tColor;
    });"

/* create shader and compile */
auto shader = context.make_shader();
shader->attach(vertex_shader, opengl::shader_type::vertex);
shader->attach(frag_shader, opengl::shader_type::fragment);
shader->link();
```
All OpenGL resources are contained in **std::shared_ptr** (it is up to the user to ensure that the resource is destroyed before [viewer](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/viewer.h) destruction; only a problem if you store the handle in a static variabel or as a member of an object created before the viewer, etc.).   
Similarly to a shader program, we create vertexarrays and vertexbuffers via the [context](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/opengl/context.h).
To handle generic vertex types, a static layout description is needed by [vertexbuffers](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/opengl/vertexbuffer.h) to *enable vertex attributes and set attribute pointer*
(*I am not really happy with this; may change in the future*).
```C++
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
```
With access to the layout description for the vertex type, a [vertexbuffers](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/opengl/vertexbuffer.h) (here a *colored triangle*) can be created and attached to a [vertexarray](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/opengl/vertexarray.h).   
In the render callback of the viewer we tell OpenGL to use the previously defined shader program and initiate a draw call with the defined [vertexarray](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/opengl/vertexarray.h).

<img src="https://github.com/nikolausrauch/opengl-playground/assets/13553309/8424517e-3121-4825-9358-7ec4e1486821" align="right" height=280px>

```C++
auto vao = context.make_vertexarray();
auto vertexbuffer = context.make_vertexbuffer<vertex>(
            std::initializer_list<vertex>
            {{ {-0.5, -0.5, 0.5}, {1.0, 0.0, 0.0, 1.0} },
             { { 0.5, -0.5, 0.5}, {0.0, 1.0, 0.0, 1.0} },
             { { 0.0,  0.5, 0.5}, {0.0, 0.0, 1.0, 1.0} } });
vao->attach(vertexbuffer);

view.on_render([&](auto& window, double dt)
{
    shader->bind();
    vao->draw(opengl::primitives::triangles);
});
```

---

## Camera and Model loading
The viewer maintains a [camera](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/utility/camera.h) instance which supports *perspective* and *orthognal* projections, and provides convinient control methods to set its position and orientation.   
By default the camera can move on an orbit around its lookat position (implemented in [orbital_control](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/utility/camera_control.h)); custom camera controller can be implemented by inheriting from **camera_control**.  
```C++
auto& camera = view.camera();
camera.position({1.75, 0.0, 0.0f});
camera.lookat({0.0, 0.0, 0.0f});
view.camera_control<util::orbit_control>();
```

[asset::model_loader](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/asset/obj_model.h) provides a functionality to load an **obj** model with *generic* material and vertex types.
The member variables need to adhere to predefined types and names (all possible options are listed in [asset/model.h](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/asset/model.h)).
```C++
struct material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

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
```
All necessary buffers and texture resources are created automatically (asset: "Sad toaster" [Link](https://skfb.ly/on9Dn) by tasha.lime.).
```C++
auto& context = view.context();
auto model = asset::model_loader<vertex, material>::load_obj(context, "assets/sad_toaster/sad_toaster.obj");
```
The render model is **ordered based on materials**:   
For each **material** the model stores **records** which hold a reference to a mesh and the face list (index offset and counter) for this material.
To draw the model you need to iterate over all materials (set the specific uniforms for this material) and then iterate over all records and initiate the drawcalls for the specified faces.

<img src="https://github.com/nikolausrauch/opengl-playground/assets/13553309/46ea3194-0e77-45a5-a133-5188d6d7d5bf" align="right" height=485px>

```C++
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
            record.m_mesh.get().vao()->draw(record.m_offset, record.m_count,
                                            opengl::primitives::triangles);
        }
    }
});
```
