#include <glad/glad.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <obj_loader.h>
#include <dynamic_array.h>
#include "model.h"

#define MAX_SHADER_BUF 1024

static int compile_shader(const char *vs_path, const char *fs_path);
static void create_vertex_buffer(Model *, Obj* obj);

Model *model_create(const char *vs_path, const char *fs_path, const char *obj_path){
    Model* m = malloc(sizeof(*m));
    Obj* obj = obj_load(obj_path);
    if(!(m->shader = compile_shader(vs_path, fs_path))){
        return NULL;
    }
    m->n_indices = da_len(obj->da_indices);
    create_vertex_buffer(m, obj);
    obj_free(&obj);
    return m;
}

static int compile_shader(const char *vs_path, const char *fs_path){
    char vs_src[MAX_SHADER_BUF], fs_src[MAX_SHADER_BUF];
    {
        FILE *vs_file, *fs_file;
        vs_file = fopen(vs_path, "r");
        fs_file = fopen(fs_path, "r");
        if(!vs_file || !fs_file){
            fprintf(stderr, "%s\n", "unable to open shader file");
            return false;
        }

        int c;
        char *vs_p = vs_src;
        while((c = fgetc(vs_file)) != EOF){
            *(vs_p++)=c;
        }
        *vs_p = '\0';

        char *fs_p = fs_src;
        while((c = fgetc(fs_file)) != EOF){
            *(fs_p++)=c;
        }
        *fs_p = '\0';

        fclose(vs_file);
        fclose(fs_file);
    }
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);


    char const * const vs_src_p = &vs_src[0];
    char const * const fs_src_p = &fs_src[0];
    glShaderSource(vs, 1, &vs_src_p, NULL);
    glShaderSource(fs, 1, &fs_src_p, NULL);

    glCompileShader(vs);
    glCompileShader(fs);

    int success;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if(!success){
        char msg[512];
        glGetShaderInfoLog(vs, 512, NULL, &msg[0]);
        fprintf(stderr, "%s\n", msg);
        return 0;
    }
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if(!success){
        char msg[512];
        glGetShaderInfoLog(fs, 512, NULL, &msg[0]);
        fprintf(stderr, "%s\n", msg);
        return 0;
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success){
        char msg[512];
        glGetProgramInfoLog(program, 512, NULL, &msg[0]);
        fprintf(stderr, "%s\n", msg);
        return 0;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
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
    glDeleteShader((*m)->shader);
    free(*m);
    *m=NULL;
}
