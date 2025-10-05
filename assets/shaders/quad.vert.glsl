#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

// We only need one uniform for all transformations!
uniform mat4 transform;
uniform float rotation;

void main()
{
    // Apply the combined model-projection matrix to the vertex position.
    gl_Position = transform * vec4(aPos, 0.0, 1.0);

    // Pass the texture coordinate to the fragment shader.
    TexCoord = aTexCoord;
}
