#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

uniform sampler2D text;

out vec4 _gl_FragColor;
in vec2 texCoord;
in vec4 tintCol;

void main()
{
    _gl_FragColor = (texture(text, texCoord) * tintCol);
}

