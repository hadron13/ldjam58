#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform float time;
uniform sampler2D tex;
uniform sampler2D normalMap;
uniform vec2 resolution;
uniform int hasNormal;

void main() {

    vec3 light_direction = normalize(vec3(sin(time/10.0), 0, cos(time/10.0)));
    vec3 normal = vec3(0, 0, 1.0);
    if(hasNormal == 1){ 
        normal = (texture(normalMap, TexCoord).rgb - 0.5) * 2.0;
    }


    float diffuse_factor = clamp(dot(normal, light_direction) * 2.0, 0, 0.7);

    FragColor = texture(tex, TexCoord) * vec4(vec3(diffuse_factor + 0.3), 1.0);
};
