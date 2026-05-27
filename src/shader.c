#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <glad/glad.h>
#include <dynamic_array.h>
#include "shader.h"

#define MAX_SHADER_BUF 1024
#define MAX_PATH_BUF 100
#define MAX_UNIFORM_KEYS 20
#define MAX_UNIFORM_KEY_BUF 30

typedef struct {
    char key[MAX_UNIFORM_KEY_BUF];
    GLint location;
    enum uniform_type type;
} UniformLocation;

typedef struct {
    char path[MAX_PATH_BUF];
    UniformLocation *da_uniform_locations;
} ShaderMetadata;

static int compile_shader(const char *vs_path, const char *fs_path);

Shader* shader_create(const char* vs_path, const char *fs_path, ShaderUniform const * const arg_uniforms, size_t arg_uniforms_len){
    ShaderMetadata* const sm = malloc(sizeof(ShaderMetadata) + sizeof(Shader));
    sm->da_uniform_locations = NULL;

    snprintf(sm->path, sizeof(sm->path),"vs: %s, fs: %s", vs_path, fs_path);
    Shader* const s = (Shader *)(sm+1);
    if(!(s->program = compile_shader(vs_path, fs_path))){
        exit(1);
    }

    //3 are how many base uniforms
    ShaderUniform uniforms[MAX_UNIFORM_KEYS + 3] = {{
        .key = SHADER_BASE_U_MODEL_MAT,
        .type = UNIFORM_MAT4,
    }, {
        .key = SHADER_BASE_U_VIEW_MAT,
        .type = UNIFORM_MAT4,
    }, {
        .key = SHADER_BASE_U_PROJECTION_MAT,
        .type = UNIFORM_MAT4,
    }};

    for(int i=0; (i<arg_uniforms_len) && (i < (MAX_UNIFORM_KEYS - 3)); i++){
        uniforms[i+2] = arg_uniforms[i];
    }

    for(int i=0; i< arg_uniforms_len + 3; i++){
        GLint loc = glGetUniformLocation(s->program, uniforms[i].key);
        if(loc < 0){
            fprintf(stderr,"%s is not defined in shader {%s}\n", uniforms[i].key, sm->path);
            exit(1);
        }
        UniformLocation uloc = {
            .location = loc,
            .type = uniforms[i].type
        };
        snprintf(uloc.key, MAX_UNIFORM_KEY_BUF, "%s", uniforms[i].key);
        da_push(sm->da_uniform_locations, uloc);
    }
    return s;
}

void shader_set_uniform(Shader * s, const char *const key, void * value){
    ShaderMetadata* sm = ((ShaderMetadata *)s) - 1;
    UniformLocation* ul = sm->da_uniform_locations;
    UniformLocation* match = NULL;
    for(int i=0; i < da_len(ul); i++){
        if(strcmp(ul[i].key, key) == 0){
            match = &ul[i];
        }
    }
    if(!match){
        fprintf(stderr, "Shader uniform with key {%s} not found at shader with path {%s}\n", key, sm->path);
        exit(1);
    }
    switch(match->type){
        case UNIFORM_MAT4 :
            glUniformMatrix4fv(match->location, 1, GL_FALSE, value);
            break;
        default :
            fprintf(stderr, "Unrecognized uniform type\n");
    }
}


//TODO(ryedough): implement ref count, to only free this after all model been freed
void shader_free(Shader **const s){
    glDeleteShader((*s)->program);
    ShaderMetadata* sm = ((ShaderMetadata*) (*s))-1;
    da_free(sm->da_uniform_locations);
    free(sm);
    *s = NULL;
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
