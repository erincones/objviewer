#version 330 core

// Location variables
layout (location = 0) in vec3 l_position;
layout (location = 1) in vec2 l_uv_coord;


// Out variables
out vec2 uv_coord;


// Main function
void main() {
    // Set out variables
    uv_coord = l_uv_coord;

    // Set vertex position
    gl_Position = vec4(l_position, 1.0F);
}