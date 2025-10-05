#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "render.h"
#include <stdio.h>


char *load_file(const char *path){
    FILE *file = fopen(path, "rb");

    if (file == NULL){
        // printf("error opening file %s", path);
        perror("error");
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);  

    char *string = malloc(file_size + 1);
    if(fread(string, file_size, 1, file) == 0){
        return NULL;
    }

    string[file_size] = 0;
    fclose(file);
    
    return string;
}

// helper
int compile_shader(int type, const char *path) {
    const char *source = load_file(path);
    if(source == NULL){
        return 0;
    }

    int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char shader_log[512];
        glGetShaderInfoLog(shader, 512, NULL, shader_log);
        printf("Shader compilation failed: %s\n", shader_log);
    }
    return shader;
}

shader_t shader_compile(const char *vertex_path, const char *fragment_path){ 

    int vertex = compile_shader(GL_VERTEX_SHADER, vertex_path);
    int fragment = compile_shader(GL_FRAGMENT_SHADER, fragment_path);

    // link shader program
    int program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    // cleanup
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}

texture_t texture_load(const char *path) {
    stbi_set_flip_vertically_on_load(1); // for opengl

    int w, h, c;
    unsigned char *data = stbi_load(path, &w, &h, &c, 4);
    if (!data) printf("Couldn't load texture: %s\n", path);

    int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return (texture_t)texture;
}

extern int viewport_w, viewport_h;


static unsigned int VAO = 0;
static unsigned int VBO = 0;
static int setup_done = 0;

void draw_quad(int shader_program, int texture, int normal_tex,
                   float x, float y, float width, float height, float rotation_radians,
                   float window_width, float window_height) {
    if (!setup_done) {
        float vertices[] = {
            0.0f, 1.0f,      0.0f, 0.0f,
            0.0f, 0.0f,      0.0f, 1.0f,
            1.0f, 0.0f,      1.0f, 1.0f,

            0.0f, 1.0f,      0.0f, 0.0f,
            1.0f, 0.0f,      1.0f, 1.0f,
            1.0f, 1.0f,      1.0f, 0.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        setup_done = 1;
    }

    glUseProgram(shader_program);

    mat4 projection = GLM_MAT4_IDENTITY_INIT;
    glm_ortho(0.0f, window_width, window_height, 0.0f, -1.0f, 1.0f, projection);

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    glm_translate(model, (vec3){x, y, 0.0f});
    glm_translate(model, (vec3){0.5f * width, 0.5f * height, 0.0f});
    glm_rotate(model, rotation_radians, (vec3){0.0f, 0.0f, 1.0f});
    glm_translate(model, (vec3){-0.5f * width, -0.5f * height, 0.0f});

    glm_scale(model, (vec3){width, height, 1.0f});

    mat4 transform;
    glm_mat4_mul(projection, model, transform);

    int transformLoc = glGetUniformLocation(shader_program, "transform");
    if (transformLoc != -1) {
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, (float*)transform);
    }

    int texLoc = glGetUniformLocation(shader_program, "tex");
    if (texLoc != -1) {
        glUniform1i(texLoc, 0);
    }
    int hasNormalLoc = glGetUniformLocation(shader_program, "hasNormal");
    if(hasNormalLoc != 0){
        glUniform1i(hasNormalLoc, normal_tex == 0? 0 : 1);
    } 

    if(normal_tex != 0){
        int normalLoc = glGetUniformLocation(shader_program, "normalMap");
        if (normalLoc != -1) {
            glUniform1i(normalLoc, 1);
        }
    }

    int resolutionLoc = glGetUniformLocation(shader_program, "resolution");
    if (resolutionLoc != -1) {
        glUniform2f(resolutionLoc, viewport_w, viewport_h);
    }

    int timeLoc = glGetUniformLocation(shader_program, "time");
    if (timeLoc != -1) {
        glUniform1f(timeLoc, (float)SDL_GetTicks()/1000.0);
    }

    int rotationLoc = glGetUniformLocation(shader_program, "rotation");
    if (rotationLoc != -1) {
        glUniform1f(rotationLoc, rotation_radians);
    }


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normal_tex);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
