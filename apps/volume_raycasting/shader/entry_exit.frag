#version 330

in vec3 tPosition;

/* https://www.khronos.org/opengl/wiki/Fragment_Shader (under Outputs (see glDrawBuffers of fragmentshader)) */
layout(location = 0) out vec3 entry_pos;
layout(location = 1) out vec3 exit_pos;

void main(void)
{
    /* https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/gl_FrontFacing.xhtml */
    if(gl_FrontFacing)
    {
        exit_pos = vec3(0, 0, 0);
        entry_pos = tPosition + 0.5;
    }
    else
    {
        entry_pos = vec3(0, 0, 0);
        exit_pos = tPosition + 0.5;
    }
}
