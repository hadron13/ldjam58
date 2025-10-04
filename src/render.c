#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "render.h"
#include <stdio.h>

static char* vertex_shader = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "void main() {\n"
    "    TexCoord = aTexCoord;\n"
    "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "}\0";

static char* fragment_shader = "#version 330 core\n"
    "in vec2 TexCoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D placeholder_texture;\n"
    "void main() {\n"
    "    FragColor = texture(placeholder_texture, TexCoord);\n"
    "}\0";


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
    fread(string, file_size, 1, file);

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

static int VAO = 0, VBO = 0, EBO = 0;
static int setup_done = 0;

void draw_quad(int shader_program, int texture, float x, float y, float width, float height) {
    if (!setup_done) {
        float vertices[] = {
            // Positions     // Tex Coords (full UV)
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };
        unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        setup_done = 1;
    }

    glUseProgram(shader_program);
    int texLoc = glGetUniformLocation(shader_program, "placeholder_texture");  // Or "ourTexture"?
    if (texLoc != -1) {
        glUniform1i(texLoc, 0);
    }

    // TODO: model matrix
    //Mat4 model;
    float window_width = 800.0f;   // Get from SDL_GetWindowSize(window, &ww, &wh);
    float window_height = 600.0f;
    //mat4_model(model, x, y, width, height, window_width, window_height);

    int modelLoc = glGetUniformLocation(shader_program, "model");
    /*if (modelLoc != -1) {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
    } else {
        printf("Warning: 'model' uniform not found!\n");
    }*/

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
