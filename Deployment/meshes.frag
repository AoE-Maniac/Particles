#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

out vec4 _gl_FragColor;

void main()
{
    _gl_FragColor = vec4(1.0);
}

