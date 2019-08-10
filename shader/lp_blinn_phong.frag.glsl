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

uniform vec3 u_ambient;
uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform float u_shininess;

uniform vec3 u_view_pos;

uniform vec3 u_background_color;

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
        color = vec4(u_background_color, 1.0F);
        return;
    }

    // Get the position, ambient and specular data from the buffer textures
    vec4 position_shininess = texture(u_position_tex, uv_coord);
    vec3 ambient            = texture(u_ambient_tex, uv_coord).rgb;
    vec3 specular           = texture(u_specular_tex, uv_coord).rgb;

    // Decompose diffuse and specular data
    vec3 position   = position_shininess.rgb;
    vec3 diffuse    = diffuse_alpha.rgb;
    float shininess = position_shininess.a * u_shininess;


    // Lambert factor
    vec3 view_dir = normalize(u_view_pos - position);
    float nl = max(dot(normal, u_light_direction), 0.0F);

    // Specular Blinn-Phong
    vec3 halfway = normalize(u_light_direction + view_dir);
    float nh = max(dot(normal, halfway), 0.0F);
    float blinn_phong = pow(nh, shininess);


    // Calcule color components
    ambient  *= u_ambient;
    diffuse  *= u_diffuse  * nl;
    specular *= u_specular * blinn_phong;


    // Attenunation and intensity for non directional light
    float attenuation = 1.0F;
    float intensity = 1.0F;
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


    // Final light
    vec3 lighting = attenuation * (ambient + intensity * (diffuse + specular));

    // Set the color
    color = vec4(lighting, 1.0F);
}