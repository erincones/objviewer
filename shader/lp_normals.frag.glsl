#version 330 core

// Out color
out vec4 color;


// Uniform variables
uniform sampler2D u_normal_tex;


// In variables
in vec2 uv_coord;


// Main function
void main() {
    // Get the data from the buffer textures
    vec4 normal_disp = texture(u_normal_tex, uv_coord);

    // Discard if the normal is black
    if ((normal_disp.x == 0.0F) && (normal_disp.y == 0.0F) && (normal_disp.z == 0.0F)) {
        discard;
    }

    // Set the color
    color = vec4(normal_disp.rgb, 1.0F);
}