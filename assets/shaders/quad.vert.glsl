#version 330

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoord;

uniform vec2 resolution;
uniform vec4 rect;

void main(){
    TexCoord = aTexCoord;
    gl_Position = vec4((aPos * rect.zw/resolution) - 1.0 + ((rect.xy+rect.zw)/resolution), 0.0, 1.0);
}
