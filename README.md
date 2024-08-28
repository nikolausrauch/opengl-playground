<h1 align="center">OpenGL Playground</h1>

This repository contains proof-of-concept implementations of various rendering / simulation techniques in OpenGL that I wrote over the past few years. 

- Implemented Examples
  - [x] [pointcloud rendering](https://github.com/nikolausrauch/opengl-playground/blob/main/apps/pointcloud_viewer) (geometry shader constructed billboards)
  - [ ] volume raycasting
  - [ ] cloth physics (position based dynamics in compute shader)
  - [x] [shadow-mapping](https://github.com/nikolausrauch/opengl-playground/blob/main/apps/shadow_mapping)
  - [x] [cel-shading (post-process edge detection)](https://github.com/nikolausrauch/opengl-playground/blob/main/apps/cel_shading)
  - [ ] physically based rendering (+ image based lighting)
  - [ ] deferred rendering
  - [ ] screen space ambient occlusion
  - [ ] screen space reflections
  - [ ] transclucent shadow map
  - [ ] texture space subsurface scattering
  - [ ] voxelizer (compute shader)
  - [ ] single-pass voxelizer

## Selected Examples

### Pointcloud Rendering (Geometry Shader Billboards)

["1982 Porsche 911 - SiteScape 3D Scan"](https://sketchfab.com/3d-models/1982-porsche-911-sitescape-3d-scan-7e315942b6db428194314285183b5896) by [SiteScape](https://skfb.ly/6XZZM) licensed under CC BY 4.0
![pc_example](https://github.com/nikolausrauch/opengl-playground/assets/13553309/8eaa0438-dfc5-49e7-b5cc-e1c5c4bc85ad)

### Shadow Mapping of Directional Light Source (PCF)
["Small City Free Pack](https://skfb.ly/6RTGw) by [mattshield](https://sketchfab.com/mattshield) licensed under CC BY 4.0.
![shadow_mapping](https://github.com/nikolausrauch/opengl-playground/assets/13553309/17f93f37-ed7b-4ab0-b863-6e809f31ab99)


## Minimal Code Example
The [viewer](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/viewer.h) class handles window (context) creation, input events, and callback registration (rendering, input, window events, and *ImGui* / *ImPlot* draw calls) - have a look at the [viewer_demos](https://github.com/nikolausrauch/opengl-playground/tree/main/apps/00_demo_viewer) for a more detailed overview.  
```C++
viewer::window_settings settings;
settings.title = "Colored Cube";
settings.width = 720;
settings.heigth = 720;

/* construct viewer (creates window and context) */
viewer view(settings);
```
The OpenGL context is created on [viewer](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/viewer.h) construction and is destroyed on destruction - OpenGL calls are only allowed in between.
For ease of use I wrapped some of the most commonly used OpenGL functionalities (not feature complete), which is accessed through an [opengl::context](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/opengl/context.h) instance.
For example, to create a [shader program](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/opengl/shaderprogram.h) the context provides a handle to which the source code can be attached, linked and compiled.
```C++
const char *vertex_shader = "
    #version 330\n
    layout(location = 0) in vec3 aPosition;
    layout(location = 1) in vec4 aColor;

    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProj;

    out vec4 tColor;

    void main(void)
    {
        gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);
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
To create [vertexbuffers](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/opengl/vertexbuffer.h) with generic vertex types, a static layout description is required (*I am not really happy with this; may change in the future*).
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
With access to the layout description, a [vertexbuffer](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/opengl/vertexbuffer.h) (here a *colored cube* + indexbuffer) can be created and attached to a [vertexarray object](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/opengl/vertexarray.h).   
```C++
/* create vertexarray, and attach vertexbuffer and indexbuffer to it */
auto vao = context.make_vertexarray();
auto vertexbuffer = context.make_vertexbuffer<vertex>(
            std::initializer_list<vertex>
            {{{-1.0, -1.0, 1.0}, {1.0, 0.0, 0.0, 1.0}}, {{-1.0,  1.0, 1.0}, {0.0, 1.0, 0.0, 1.0}},
             {{ 1.0,  1.0, 1.0}, {0.0, 0.0, 1.0, 1.0}}, {{ 1.0, -1.0, 1.0}, {1.0, 0.0, 1.0, 1.0}},

             {{-1.0, -1.0, -1.0}, {1.0, 0.0, 0.0, 1.0}}, {{-1.0,  1.0, -1.0}, {0.0, 1.0, 0.0, 1.0}},
             {{ 1.0,  1.0, -1.0}, {0.0, 0.0, 1.0, 1.0}}, {{ 1.0, -1.0, -1.0}, {1.0, 0.0, 1.0, 1.0}}});

auto indexbuffer = context.make_indexbuffer<unsigned int>(
            std::initializer_list<unsigned int>
            {0, 2, 1,   2, 0, 3,
             4, 5, 6,   6, 7, 4,
             0, 1, 5,   5, 4, 0,
             3, 6, 2,   6, 3, 7,
             1, 6, 5,   6, 1, 2,
             0, 4, 7,   7, 3, 0});
vao->attach(vertexbuffer);
vao->attach(indexbuffer);
```
In the render callback of the [viewer](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/viewer.h) we use the previously defined [shader program](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/opengl/shaderprogram.h) to draw the content of the [vertexarray](https://github.com/nikolausrauch/opengl-playground/blob/main/viewer/opengl/vertexarray.h).

<img src="https://github.com/nikolausrauch/opengl-playground/assets/13553309/a31e5df8-a1eb-44c6-a609-965f72ad9ca6" align="right" height=374px>

```C++
/* access viewer camera */
auto& cam = view.camera();

/* set render callback*/
view.on_render([&](auto& window, float dt)
{
    static float s_time = 0.0f;
    s_time += dt;

    /* set shader uniforms */
    shader->bind();
    shader->uniform("uModel", glm::rotate(s_time*glm::pi<float>(),
                              glm::vec3{0.0f, 1.0f, 0.0f}));
    shader->uniform("uView", cam.view());
    shader->uniform("uProj", cam.projection());

    /* draw elements */
    vao->draw(opengl::primitives::triangles);
});

/* execute main loop */
view.run();
```