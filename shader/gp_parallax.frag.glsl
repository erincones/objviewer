#version 330 core

// Parallax mapping layers macros
#define MAX_LAYERS 32.0F
#define MIN_LAYERS  8.0F

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
uniform float u_displacement;

uniform sampler2D u_ambient_tex;
uniform sampler2D u_diffuse_tex;
uniform sampler2D u_specular_tex;
uniform sampler2D u_shininess_tex;
uniform sampler2D u_normal_tex;
uniform sampler2D u_displacement_tex;


// In variables
in Vertex {
    vec3 position;
    vec2 uv_coord;
    vec3 tangent_pos;
} vertex;

in vec3 tangent_view_pos;

in mat3 tbn;


// Main function
void main () {
    // Texture coordinates
    vec2 uv_coord = vertex.uv_coord;

    // Parallax mapping
    float mapped_depth = texture(u_displacement_tex, uv_coord).r;
    if ((mapped_depth * u_displacement) != 0.0F) {
        // Tangent view direction
        vec3 view_dir = normalize(tangent_view_pos - vertex.tangent_pos);

        // Number of layers for parallax mapping
        const vec3 up = vec3(0.0F, 0.0F, 1.0F);
        float layers = mix(MAX_LAYERS, MIN_LAYERS, abs(dot(up, view_dir)));
        float layer_depth = 1.0F / layers;

        // Initialize variables for parallax mapping
        vec2 disp = (view_dir.xy / view_dir.z) * u_displacement;
        vec2 delta_depth = disp / layers;
        float depth = 0.0F;

        // Steep parallax mapping
        while (depth < mapped_depth) {
            depth += layer_depth;
            uv_coord -= delta_depth;
            mapped_depth = texture(u_displacement_tex, uv_coord).r;
        }

        // Before and after depth 
        vec2 prev_steep = uv_coord + delta_depth;
        float before_depth = texture(u_displacement_tex, prev_steep).r - depth + layer_depth;
        float after_depth = mapped_depth - depth;

        // Interpolate texture coordinates
        float weight = after_depth / (after_depth - before_depth);
        uv_coord = prev_steep * weight + uv_coord * (1.0F - weight);

        // Discard overflowed uv coordinates
        if ((uv_coord.s > 1.0F) || (uv_coord.s < 0.0F) || (uv_coord.t > 1.0F) || (uv_coord.t < 0.0F)) {
            discard;
        }
    }


    // Fragment position
    l_position = vertex.position;

    // Normal
    l_normal = tbn * normalize(normalize(texture(u_normal_tex, uv_coord).rgb) * 2.0F - 1.0F);

    // Ambient color
    l_ambient = texture(u_ambient_tex, uv_coord).rgb * u_ambient;

    // Diffuse color and alpha
    l_diffuse = texture(u_diffuse_tex, uv_coord);
    l_diffuse.rgb *= u_diffuse;
    l_diffuse.a   *= u_alpha;

    // Specular color
    l_specular = texture(u_specular_tex, uv_coord).rgb * u_specular;


    // Metadata

    // Shininess
    l_metadata.x = texture(u_shininess_tex, uv_coord).r * u_shininess;

    // Roughness
    l_metadata.y = u_roughness;

    // Metalness
    l_metadata.z = u_metalness;

    // Refractive index
    l_metadata.w = 0.0F;
}