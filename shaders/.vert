#version 330 core

layout (location=0) in vec3 aPos;

uniform mat4 u_model_mat;
uniform mat4 u_view_mat;
uniform mat4 u_projection_mat;

void main(){
    gl_Position = u_projection_mat * u_view_mat *  u_model_mat * vec4(aPos, 1.0f);
}

