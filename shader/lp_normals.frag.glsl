#version 330 core

// Out color
out vec4 color;


// Uniform variables
uniform vec3 u_background_color;

uniform sampler2D u_normal_tex;


// In variables
in vec2 uv_coord;


// Main function
void main() {
    // Get the data from the buffer textures
    vec3 normal = texture(u_normal_tex, uv_coord).rgb;

    // Discard if the normal is black
    if (normal == vec3(0.0F)) {
        color = vec4(u_background_color, 1.0F);
        return;
    }

    // Set the normal as color
    color = vec4(normal, 1.0F);
}