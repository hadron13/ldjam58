#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Tangent;
out vec3 BiTangent;

uniform mat4 transform;
uniform float rotation;

void main()
{
    gl_Position = transform * vec4(aPos, 0.0, 1.0);

    TexCoord = aTexCoord;
    Tangent   = vec3(sin(rotation), cos(rotation), 0.0);
    BiTangent = cross(Tangent, vec3(0.0, 0.0, 1.0));
}
