#version 330 core

// Location variables
layout (location = 0) in vec3 l_position;
layout (location = 1) in vec2 l_uv_coord;
layout (location = 2) in vec3 l_normal;
layout (location = 3) in vec3 l_tangent;


// Uniform variables
uniform vec3 u_view_pos;

uniform mat4 u_view_mat;
uniform mat4 u_projection_mat;

uniform mat4 u_model_mat;
uniform mat3 u_normal_mat;


// Out variables
out Vertex {
    vec3 position;
    vec2 uv_coord;
    vec3 tangent_pos;
} vertex;

out vec3 tangent_view_pos;

out mat3 tbn;


// Main function
void main() {
    // Vertex position
    vec4 pos = u_model_mat * vec4(l_position, 1.0F);

    // Build the TBN matrix
    vec3 t = u_normal_mat * l_tangent;
    vec3 n = u_normal_mat * l_normal;
    vec3 b = normalize(cross(n, t));
    tbn = mat3(t, b, n);

    // Set out variables
    vertex.position    = pos.xyz;
    vertex.uv_coord    = l_uv_coord;
    vertex.tangent_pos = vertex.position * tbn;

    tangent_view_pos = u_view_pos * tbn;


    // Set vertex position
    gl_Position = u_projection_mat * u_view_mat * pos;
}