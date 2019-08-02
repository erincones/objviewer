#version 330 core

// In variables
in Vertex {
    vec3 position;
    vec2 uv_coord;
    vec3 normal;
} vertex;

// Out color
out vec4 color;


// Main function
void main() {
    color = vec4(vertex.normal, 1.0F);
}