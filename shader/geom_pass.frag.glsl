#version 330 core

// Location variables
layout (location = 0) out vec4 l_position_shininess;
layout (location = 1) out vec4 l_normal_displacement;
layout (location = 2) out vec3 l_ambient;
layout (location = 3) out vec4 l_diffuse_alpha;
layout (location = 4) out vec3 l_specular;


// Uniform variables
uniform vec3 u_ambient;
uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform float u_shininess;

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
    // Fragment position and shininess
    l_position_shininess.rgb = vertex.position;
    l_position_shininess.a = texture(u_shininess_tex, vertex.uv_coord).r * u_shininess;

    // Normal and displacement
    l_normal_displacement.rgb = normalize(vertex.normal);
    l_normal_displacement.a = 0.0F;

    // Ambient color
    l_ambient = texture(u_ambient_tex, vertex.uv_coord).rgb * u_ambient;

    // Diffuse color and alpha
    l_diffuse_alpha = texture(u_diffuse_tex, vertex.uv_coord);
    l_diffuse_alpha.rgb *= u_diffuse;

    // Specular color
    l_specular = texture(u_specular_tex, vertex.uv_coord).rgb * u_specular;
}