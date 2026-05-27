#pragma once
#include <GLFW/glfw3.h>

typedef struct{
    size_t n_indices;
    GLint shader;
    GLuint vao, ebo, vbo;
} Model;

Model *model_create(const char* vs_path, const char* fs_path, const char* obj_path);
void model_free(Model ** const model);
