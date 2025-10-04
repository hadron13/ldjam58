#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D placeholder_texture;
void main() {
    FragColor = texture(placeholder_texture, TexCoord);
};
