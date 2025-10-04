#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D tex;

uniform vec2 resolution;


void main() {



    FragColor = vec4(0.5, 0.5, 0.5, 1.0);
};
