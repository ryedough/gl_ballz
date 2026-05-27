#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stdbool.h>
#include <cglm/struct.h>
#include <stdio.h>
#include "model.h"
#include "shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main(){
    GLFWwindow* window;

    if(!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(640, 480, "GL Ballz", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        glfwTerminate();
        return -1;
    }
    glViewport(0, 0, 640, 480);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Shader *basic_shader = shader_create("./shaders/.vert", "./shaders/.frag", NULL, 0);
    Model *basic = model_create("./assets/ball.obj", basic_shader);
    if(!basic_shader){
        glfwTerminate();
        return -1;
    }
    mat4 projection_mat, view_mat;
    glm_mat4_identity(view_mat);
    glm_translate(view_mat, (float[3]){0., 0., -3.});

    // glm_mat4_identity(projection_mat);
    glm_perspective(glm_rad(45.0f), (float)640/(float)480, 0.1f, 100.f, projection_mat);

    glClearColor(0.0f, 0.2f, 0.4f, .0f);
    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(basic->shader->program);
        shader_set_uniform(basic->shader, SHADER_BASE_U_MODEL_MAT, basic->model_mat);
        shader_set_uniform(basic->shader, SHADER_BASE_U_VIEW_MAT, view_mat);
        shader_set_uniform(basic->shader, SHADER_BASE_U_PROJECTION_MAT, projection_mat);
        glBindVertexArray(basic->vao);
        glDrawElements(GL_TRIANGLES, basic->n_indices, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
