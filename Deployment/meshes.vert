#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

in mat4 MVP;
in vec3 pos;
in vec2 tex;
in vec3 nor;

void main()
{
    gl_Position = (MVP * vec4(pos, 1.0));
}

