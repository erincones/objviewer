#version 330 core

// Location variables
layout (location = 0) out vec3 l_position;
layout (location = 1) out vec3 l_normal;
layout (location = 2) out vec3 l_ambient;
layout (location = 3) out vec4 l_diffuse;
layout (location = 4) out vec3 l_specular;
layout (location = 5) out vec4 l_metadata;


// Uniform variables
uniform vec3 u_ambient;
uniform vec3 u_diffuse;
uniform vec3 u_specular;

uniform float u_shininess;
uniform float u_roughness;
uniform float u_metalness;
uniform float u_alpha;

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
    l_normal = normalize(vertex.normal);

    // Ambient color
    l_ambient = texture(u_ambient_tex, vertex.uv_coord).rgb * u_ambient;

    // Diffuse color and alpha
    l_diffuse = texture(u_diffuse_tex, vertex.uv_coord);
    l_diffuse.rgb *= u_diffuse;
    l_diffuse.a   *= u_alpha;

    // Specular color
    l_specular = texture(u_specular_tex, vertex.uv_coord).rgb * u_specular;


    // Metadata

    // Shininess
    l_metadata.x = texture(u_shininess_tex, vertex.uv_coord).r * u_shininess;

    // Roughness
    l_metadata.y = u_roughness;

    // Metalness
    l_metadata.z = u_metalness;

    // Refractive index
    l_metadata.w = 0.0F;
}