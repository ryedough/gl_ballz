#pragma once
#include <stddef.h>
#include <glad/glad.h>
#include "cglm/types.h"
#define SHADER_BASE_U_VIEW_MAT "u_view_mat"
#define SHADER_BASE_U_PROJECTION_MAT "u_projection_mat"
#define SHADER_BASE_U_MODEL_MAT "u_model_mat"

enum uniform_type {
    UNIFORM_MAT4,
};

typedef struct {
    const char* key;
    enum uniform_type type;
} ShaderUniform;

typedef struct {
    GLint program;
} Shader;

Shader* shader_create(const char* vs_path, const char *fs_path, ShaderUniform const * const uniforms, size_t uniforms_len);
void shader_set_uniform(Shader*, const char* const key, void* value);
void shader_free(Shader** const);
