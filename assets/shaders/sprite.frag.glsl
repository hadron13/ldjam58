#version 330 core
in vec2 TexCoord;
in vec3 Tangent;
in vec3 BiTangent;
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
        vec3 normal_tex= (texture(normalMap, TexCoord).rgb - 0.5) * 2.0;
        normal = (normal_tex.x * BiTangent + normal_tex.y * Tangent + normal_tex.z * normal);
    }


    float diffuse_factor = clamp(dot(normal, light_direction) * 2.0, 0, 1.0);

    vec4 color = texture(tex, TexCoord);
    if(hasNormal == 1){
        color *=  vec4(vec3(diffuse_factor + 0.1 ), 1.0);
        FragColor = vec4(pow(color.rgb, vec3(0.4545)), color.a); 
    }else{
        FragColor = vec4(color);
    }
};
