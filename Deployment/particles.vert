#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

uniform mat4 P;
uniform mat4 V;

in mat4 M;
in vec3 pos;
out vec2 texCoord;
in vec2 tex;
in vec2 texOffset;
out vec4 tintCol;
in vec4 tint;

void main()
{
    gl_Position = (((P * V) * M) * vec4(pos, 1.0));
    texCoord = ((tex + texOffset) / vec2(2.0));
    tintCol = tint;
}

