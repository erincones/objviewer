#version 330 core

// Location variables
layout (location = 0) out vec3 l_position;
layout (location = 1) out vec4 l_normal;
layout (location = 2) out vec3 l_ambient;
layout (location = 3) out vec4 l_diffuse;
layout (location = 4) out vec4 l_specular;


// Uniform variables
uniform sampler2D u_ambient_tex;
uniform sampler2D u_diffuse_tex;
uniform sampler2D u_specular_tex;
uniform sampler2D u_shininess_tex;


// In variables
in Vertex {
    vec3 position;
    vec2 uv_coord;
    vec3 normal;
} vertex;


// Main function
void main () {
    // Fragment position
    l_position = vertex.position;

    // Normal
    l_normal.rgb = normalize(vertex.normal);
    l_normal.a = 0.0F;

    // Ambient color
    l_ambient = texture(u_ambient_tex, vertex.uv_coord).rgb;

    // Diffuse color
    l_diffuse = texture(u_diffuse_tex, vertex.uv_coord);

    // Specular color
    l_specular.rgb = texture(u_diffuse_tex, vertex.uv_coord).rgb;
    l_specular.a = texture(u_shininess_tex, vertex.uv_coord).a;
}