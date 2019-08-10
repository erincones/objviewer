#version 330

// Light macros
#define DIRECTIONAL 0
#define POINT       1
#define SPOTLIGHT   2


// Out color
out vec4 color;


// Uniform variables
uniform int u_light_type;

uniform vec3 u_light_direction;
uniform vec3 u_light_position;
uniform vec3 u_light_attenuation;
uniform vec2 u_light_cutoff;

uniform vec3 u_light_ambient;
uniform vec3 u_light_diffuse;
uniform vec3 u_light_specular;

uniform float u_ambient_level;
uniform float u_diffuse_level;
uniform float u_specular_level;
uniform float u_light_shininess;

uniform vec3 u_view_pos;

uniform sampler2D u_position_tex;
uniform sampler2D u_normal_tex;
uniform sampler2D u_ambient_tex;
uniform sampler2D u_diffuse_tex;
uniform sampler2D u_specular_tex;


// In variables
in vec2 uv_coord;


// Main function
void main() {
    // Get the normal and fissue data from the buffer textures
    vec3 normal        = texture(u_normal_tex, uv_coord).rgb;
    vec4 diffuse_alpha = texture(u_diffuse_tex, uv_coord);

    // Discard if the normal is black and transparent pixels
    if ((diffuse_alpha.a == 0.0F) || (normal == vec3(0.0F))) {
        discard;
    }

    // Get the position, ambient and specular data from the buffer textures
    vec3 position           = texture(u_position_tex, uv_coord).rgb;
    vec3 ambient            = texture(u_ambient_tex, uv_coord).rgb;
    vec4 specular_shininess = texture(u_specular_tex, uv_coord);

    // Decompose diffuse and specular data
    vec3 diffuse = diffuse_alpha.rgb;
    vec3 specular   = specular_shininess.rgb;
    float shininess = specular_shininess.a;


    // View direction and initial attenuation and intensity
    vec3 view_dir = normalize(u_view_pos - position);
    float attenuation = 1.0F;
    float intensity = 1.0F;

    // Attenunation for non directional light
    if (u_light_type != DIRECTIONAL) {
        vec3 light_dir = u_light_position - position;
        float dist = length(light_dir);
        attenuation /= (u_light_attenuation.x + u_light_attenuation.y * dist + u_light_attenuation.z * dist * dist);

        // Spotlight intensity
        if (u_light_type == SPOTLIGHT) {
            float theta = dot(normalize(light_dir), u_light_direction);
            float epsilon = u_light_cutoff.x - u_light_cutoff.y;
            intensity = clamp((theta - u_light_cutoff.y) / epsilon, 0.0F, 1.0F);
        }
    }

    // Halfway vector and dot products
    vec3 halfway = normalize(u_light_direction + view_dir);
    float nl = dot(u_light_direction, normal);
    float nh = dot(normal, halfway);

    // Specular Blinn-Phong
    float blinn_phong = pow(max(nh, 0.0F), u_light_shininess * shininess);

    // Calcule components colors
    ambient  *= u_ambient_level * u_light_ambient;
    diffuse  *= max(nl, 0.0F) * u_light_diffuse;
    specular *= u_specular_level * blinn_phong * u_light_specular;

    // Final light
    vec3 lighting = attenuation * (ambient + intensity * (diffuse + specular));


    // Set the color
    color = vec4(lighting, 1.0F);
}