#version 330 core

// Location variables
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv_coord;
layout (location = 2) in vec3 normal;


// Camera
uniform mat4 view_mat;
uniform mat4 projection_mat;

// Model
uniform mat4 model_mat;
uniform mat3 normal_mat;


// Out variables
out Vertex {
	vec3 position;
	vec2 uv_coord;
	vec3 normal;
} vertex;


// Main function
void main() {
	// Vertex position
    vec4 pos = model_mat * vec4(position, 1.0F);

	// Set out variables
    vertex.position = pos.xyz;
    vertex.uv_coord = uv_coord;
    vertex.normal = normal_mat * normal;

	// Set vertex position
    gl_Position = projection_mat * view_mat * pos;
}