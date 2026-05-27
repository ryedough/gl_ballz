#pragma once
#include <stddef.h>
#include <cglm/cglm.h>
#include "shader.h"

typedef struct{
    size_t n_indices;
    GLuint vao, ebo, vbo;
    Shader* shader;
    mat4 model_mat;
} Model;

Model *model_create(const char* obj_path, Shader* shader);
void model_free(Model ** const model);
