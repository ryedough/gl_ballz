#include <glad/glad.h>
#include <stdbool.h>
#include <stdlib.h>
#include <obj_loader.h>
#include <dynamic_array.h>
#include <cglm/cglm.h>
#include "model.h"

static void create_vertex_buffer(Model *, Obj* obj);

Model *model_create(const char *obj_path, Shader* shader){
    Model* m = malloc(sizeof(*m));
    Obj* obj = obj_load(obj_path);

    m->shader = shader;
    glm_mat4_identity(m->model_mat);
    m->n_indices = da_len(obj->da_indices);
    create_vertex_buffer(m, obj);

    obj_free(&obj);
    return m;
}

static void create_vertex_buffer(Model *m, Obj* obj){
    //VAO
    glGenVertexArrays(1, &m->vao);
    glBindVertexArray(m->vao);

    glGenBuffers(1, &m->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glBufferData(GL_ARRAY_BUFFER, da_len(obj->da_verts) * sizeof(float), obj->da_verts, GL_STATIC_DRAW);

    glGenBuffers(1, &m->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, da_len(obj->da_indices) * sizeof(unsigned int), obj->da_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void model_free(Model **const m){
    glDeleteBuffers(1, &(*m)->ebo);
    glDeleteBuffers(1, &(*m)->vbo);
    glDeleteVertexArrays(1, &(*m)->vao);
    free(*m);
    *m=NULL;
}
