#version 330 core

// Location variables
layout (location = 0) in vec3 l_position;
layout (location = 1) in vec2 l_uv_coord;
layout (location = 2) in vec3 l_normal;


// Uniform variables
uniform mat4 u_view_mat;
uniform mat4 u_projection_mat;

uniform mat4 u_model_mat;
uniform mat3 u_normal_mat;


// Out variables
out Vertex {
    vec3 position;
    vec2 uv_coord;
    vec3 normal;
} vertex;


// Main function
void main() {
    // Vertex position
    vec4 pos = u_model_mat * vec4(l_position, 1.0F);

    // Set out variables
    vertex.position = pos.xyz;
    vertex.uv_coord = l_uv_coord;
    vertex.normal = u_normal_mat * l_normal;

    // Set vertex position
    gl_Position = u_projection_mat * u_view_mat * pos;
}