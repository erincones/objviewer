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
uniform sampler2D u_metadata_tex;


// In variables
in vec2 uv_coord;


// Main function
void main() {
    // Get the normal and fissue data from the buffer textures
    vec3 normal        = texture(u_normal_tex, uv_coord).rgb;
    vec4 diffuse_alpha = texture(u_diffuse_tex, uv_coord);

    // Discard black normals and transparent fragments
    if ((diffuse_alpha.a == 0.0F) || (normal == vec3(0.0F))) {
        color = vec4(u_background_color, 1.0F);
        return;
    }

    // Get the position, ambient, specular and other metadata from the buffer textures
    vec3 position = texture(u_position_tex, uv_coord).rgb;
    vec3 ambient  = texture(u_ambient_tex, uv_coord).rgb;
    vec3 specular = texture(u_specular_tex, uv_coord).rgb;
    vec4 metadata = texture(u_metadata_tex, uv_coord);

    // Decompose diffuse, shininess, roughness and metalness data
    vec3 diffuse    = diffuse_alpha.rgb;
    float shininess = metadata.x * u_shininess;
    float roughness = metadata.y;
    float metalness = metadata.z;


    // Light direction, attenuation and intensity
    vec3 light_dir;
    float attenuation;
    float intensity;

    // Directional light
    if (u_light_type == DIRECTIONAL) {
        light_dir = u_light_direction;
        attenuation = 1.0F;
        intensity = 1.0F;
    }

    // Non directional light
    else {
        // Attenuation
        light_dir = u_light_position - position;
        float dist = length(light_dir);
        attenuation = 1.0F / (u_light_attenuation.x + u_light_attenuation.y * dist + u_light_attenuation.z * dist * dist);

        // Normalize the light direction
        light_dir = normalize(light_dir);

        // Spotlight intensity
        if (u_light_type == SPOTLIGHT) {
            float theta = dot(light_dir, u_light_direction);
            float epsilon = u_light_cutoff.x - u_light_cutoff.y;
            intensity = clamp((theta - u_light_cutoff.y) / epsilon, 0.0F, 1.0F);
        }

        // Non spotlight light default intensity
        else {
            intensity = 1.0F;
        }
    }


    // Lambert factor
    vec3 view_dir = normalize(u_view_pos - position);
    float nl = dot(normal, light_dir);

    // Fresnel
    vec3 halfway = normalize(light_dir + view_dir);
    float hv = dot(halfway, view_dir);
    float hv_comp = 1.0F - hv;
    vec3 f0 = mix(ambient, specular, metalness);
    vec3 fresnel = f0 + (1.0F - f0) * hv_comp * hv_comp * hv_comp * hv_comp * hv_comp;

    // Roughness
    float nh = dot(normal, halfway);
    if (roughness > 0.0F) {
        float nh_sqr = nh * nh;
        roughness *= nh_sqr;
        roughness  = exp((nh_sqr - 1.0F) / roughness) / (roughness * nh_sqr);
    }

    // Geometry attenuation
    float nv = dot(normal, view_dir);
    float aux = 2.0F * nh / hv;
    float geometry = min(min(aux * nv, aux * nl), 1.0F);

    // Cook-Torrance
    float nvnl = nv * nl;
    vec3 cook_torrance = (nvnl != 0.0F ? (fresnel * roughness * geometry) / nvnl : vec3(0.0F));


    // Calcule color components
    ambient  *= u_ambient;
    diffuse  *= u_diffuse;
    specular *= u_specular * cook_torrance;


    // Final light
    vec3 lighting = attenuation * (ambient + intensity * max(nl, 0.0F) * (diffuse + specular));

    // Set the color
    color = vec4(lighting, diffuse_alpha.a);
}