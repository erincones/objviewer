#include "material.hpp"

#define STBI_ASSERT(x)
#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

#include <iostream>


// Private static attributes

// Default white texture
GLuint Material::white_tex = GL_FALSE;

// Default black texture
GLuint Material::black_tex = GL_FALSE;

// Default blue texture
GLuint Material::blue_tex = GL_FALSE;


// Private static methods

// Create a default texture
GLuint Material::createDefaultTexture(const glm::vec3 &color) {
    // Border color
    float border[4] = {color[0], color[1], color[2], 1.0F};

    // Generate the new texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Texture parameters
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &border[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Load texture image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_FLOAT, &color[0]);

    // Return texture
    return texture;
}

// Bind texture
void Material::bindTexture(const GLenum &index, const GLuint &texture, const GLuint &default_texture) {
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, texture != GL_FALSE ? texture : default_texture);
}


// Load a 2D texture
GLuint Material::load2DTexture(const std::string &path) {
    // Check if path is empty
    if (path.empty()) {
        return GL_FALSE;
    }

    // Image properties
    int width;
    int height;
    int channels;

    // Open image as RGBA
    stbi_set_flip_vertically_on_load(true);
    stbi_uc *data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    // Check data
    if (data == nullptr) {
        std::cerr << "error: could not open the texture `" << path << "'" << std::endl;
        return GL_FALSE;
    }

    // Generate new texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // Load texture and generate mipmap
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Free memory
    stbi_image_free(data);

    // Return texture
    return texture;
}

// Load a cube map texture
GLuint Material::loadCubeMapTexture(const std::string (&path)[6]) {
    // Image properties
    int width[6] = {1};
    int height[6] = {1};
    int channels[6];

    // Open images as RGB
    stbi_set_flip_vertically_on_load(true);
    stbi_uc *data[6];

    for (int i = 0; i < 6; i++) {
        data[i] = stbi_load(path[i].c_str(), &width[i], &height[i], &channels[i], STBI_rgb);

        // Check image data
        if (data[i] == nullptr) {
            std::cerr << "error: could not open the texture `" << path << "' (cube side " << i << ")" << std::endl;
        }
    }

    // Generate new texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // Load texture and free memory
    for (GLint i = 0; i < 6; i++) {
        if (data[i] != nullptr) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width[i], height[i], 0, GL_RGB, GL_UNSIGNED_BYTE, data[i]);
            stbi_image_free(data[i]);
        }
    }

    // Generate mipmap
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // Return texture
    return texture;
}


// Constructor

// Material constructor
Material::Material(const std::string &name) :
    // Name
    name(name),

    // Colors
    ambient_color(0.0F),
    diffuse_color(1.0F),
    specular_color(0.125F),
    transparency_color(1.0F),

    // Values
    shininess(10.0F),
    roughness(0.3F),
    metalness(0.1F),
    transparency(0.0F),
    displacement(0.05),
    refractive_index(1.0F),

    // Textures
    ambient_tex(GL_FALSE),
    diffuse_tex(GL_FALSE),
    specular_tex(GL_FALSE),
    shininess_tex(GL_FALSE),
    normal_tex(GL_FALSE),
    displacement_tex(GL_FALSE),
    cube_map_tex(GL_FALSE) {}


// Getters

// Get the material name
std::string Material::getName() const {
    return name;
}

// Get the color of the given attribute
glm::vec3 Material::getColor(const Material::Attribute &attrib) const {
    switch (attrib) {
        // Return color by attribute
        case Material::AMBIENT:      return ambient_color;
        case Material::DIFFUSE:      return diffuse_color;
        case Material::SPECULAR:     return specular_color;
        case Material::TRANSPARENCY: return transparency_color;

        // Invalid attribute
        default:
            std::cerr << "error: invalid attribute `" << attrib << "'" << std::endl;
            return glm::vec3(NAN);
    }
}

// Get the value of the given attribute
float Material::getValue(const Material::Attribute &attrib) const {
    switch (attrib) {
        // Return value by attribute
        case Material::SHININESS:        return shininess;
        case Material::ROUGHNESS:        return roughness;
        case Material::METALNESS:        return metalness;
        case Material::TRANSPARENCY:     return transparency;
        case Material::DISPLACEMENT:     return displacement;
        case Material::REFRACTIVE_INDEX: return refractive_index;

        // Invalid attribute
        default:
            std::cerr << "error: invalid attribute `" << attrib << "'" << std::endl;
            return NAN;
    }
}

// Get the texture of the given attribute
GLuint Material::getTexture(const Material::Attribute &attrib) const {
    switch (attrib) {
        // Return texture by attribute
        case Material::AMBIENT:      return ambient_tex;
        case Material::DIFFUSE:      return diffuse_tex;
        case Material::SPECULAR:     return specular_tex;
        case Material::SHININESS:    return shininess_tex;
        case Material::NORMAL:       return normal_tex;
        case Material::DISPLACEMENT: return displacement_tex;
        case Material::CUBE_MAP:     return cube_map_tex;

        // Invalid attribute
        default:
            std::cerr << "error: invalid attribute `" << attrib << "'" << std::endl;
            return GL_FALSE;
    }
}

// Get the texture path of the given attribute
std::string Material::getTexturePath(const Material::Attribute &attrib) const {
    switch (attrib) {
        // Return texture path by attribute
        case Material::AMBIENT:         return ambient_tex_path;
        case Material::DIFFUSE:         return diffuse_tex_path;
        case Material::SPECULAR:        return specular_tex_path;
        case Material::SHININESS:       return shininess_tex_path;
        case Material::NORMAL:          return normal_tex_path;
        case Material::DISPLACEMENT:    return displacement_tex_path;

        // Cube map texture paths
        case Material::CUBE_MAP_RIGHT:  return cube_map_tex_path[0];
        case Material::CUBE_MAP_LEFT:   return cube_map_tex_path[1];
        case Material::CUBE_MAP_TOP:    return cube_map_tex_path[2];
        case Material::CUBE_MAP_BOTTOM: return cube_map_tex_path[3];
        case Material::CUBE_MAP_FRONT:  return cube_map_tex_path[4];
        case Material::CUBE_MAP_BACK:   return cube_map_tex_path[5];

        // Invalid attribute
        default:
            std::cerr << "error: invalid attribute `" << attrib << "'" << std::endl;
            return "INVALID";
    }
}


// Setters

// Set the material name
void Material::setName(const std::string &new_name) {
    name = new_name;
}

// Set the color of the given attribute
void Material::setColor(const glm::vec3 &color, const Material::Attribute &attrib) {
    switch (attrib) {
        // Set color by attribute
        case Material::AMBIENT:      ambient_color      = color; return;
        case Material::DIFFUSE:      diffuse_color      = color; return;
        case Material::SPECULAR:     specular_color     = color; return;
        case Material::TRANSPARENCY: transparency_color = color; return;

        // Invalid attribute
        default:
            std::cerr << "error: invalid attribute `" << attrib << "'" << std::endl;
    }
}

// Set the value of the given attribute
void Material::setValue(const float &value, const Material::Attribute &attrib) {
    switch (attrib) {
        // Set value by attribute
        case Material::SHININESS:        shininess        = value; return;
        case Material::ROUGHNESS:        roughness        = value; return;
        case Material::METALNESS:        metalness        = value; return;
        case Material::TRANSPARENCY:     transparency     = value; return;
        case Material::DISPLACEMENT:     displacement     = value; return;
        case Material::REFRACTIVE_INDEX: refractive_index = value; return;

        // Invalid attribute
        default:
            std::cerr << "error: invalid attribute `" << attrib << "'" << std::endl;
    }
}

// Set the texture path of the given attribute
void Material::setTexturePath(const std::string &path, const Material::Attribute &attrib) {
    switch (attrib) {
        // Set texture path by attribute
        case Material::AMBIENT:      ambient_tex_path      = path; break;
        case Material::DIFFUSE:      diffuse_tex_path      = path; break;
        case Material::SPECULAR:     specular_tex_path     = path; break;
        case Material::SHININESS:    shininess_tex_path    = path; break;
        case Material::NORMAL:       normal_tex_path       = path; break;
        case Material::DISPLACEMENT: displacement_tex_path = path; break;

        // Cube map texture paths
        case Material::CUBE_MAP:
        case Material::CUBE_MAP_RIGHT: 
        case Material::CUBE_MAP_LEFT:  
        case Material::CUBE_MAP_TOP:   
        case Material::CUBE_MAP_BOTTOM:
        case Material::CUBE_MAP_FRONT: 
        case Material::CUBE_MAP_BACK:
            // Print warning message
            std::cout << "warning: using the same texture for all cube map sides, use `Material::setCubeMapTexturePath(const std::string (&path)[6])' to set each cube map path separately" << std::endl;
            
            // Set the cube map texture paths
            for (int i = 1; i < 6; i++) {
                cube_map_tex_path[i] = path;
            }
            break;  

        // Invalid attribute
        default:
            std::cerr << "error: invalid attribute `" << attrib << "'" << std::endl;
            return;
    }

    // Reload texture
    reloadTexture(attrib);
}

// Set the cube map texture path
void Material::setCubeMapTexturePath(const std::string (&path)[6]) {
    // Set the texture paths
    for (int i = 1; i < 6; i++) {
        cube_map_tex_path[i] = path[i];
    }

    // Reload texture
    reloadTexture(Material::CUBE_MAP);
}


// Methods

// Reload textures
void Material::reloadTexture(const Material::Attribute &attrib) {
    // Ambient texture
    if ((attrib & Material::AMBIENT) != 0U) {
        glDeleteTextures(1, &ambient_tex);
        ambient_tex = Material::load2DTexture(ambient_tex_path);
    }

    // Diffuse texture
    if ((attrib & Material::DIFFUSE) != 0U) {
        glDeleteTextures(1, &diffuse_tex);
        diffuse_tex = Material::load2DTexture(diffuse_tex_path);
    }

    // Specular texture
    if ((attrib & Material::SPECULAR) != 0U) {
        glDeleteTextures(1, &specular_tex);
        specular_tex = Material::load2DTexture(specular_tex_path);
    }

    // Shininess texture
    if ((attrib & Material::SHININESS) != 0U) {
        glDeleteTextures(1, &shininess_tex);
        shininess_tex = Material::load2DTexture(shininess_tex_path);
    }

    // Normal texture
    if ((attrib & Material::NORMAL) != 0U) {
        glDeleteTextures(1, &normal_tex);
        normal_tex = Material::load2DTexture(normal_tex_path);
    }

    // Displacement texture
    if ((attrib & Material::DISPLACEMENT) != 0U) {
        glDeleteTextures(1, &displacement_tex);
        displacement_tex = Material::load2DTexture(displacement_tex_path);
    }

    // Cube map texture
    if ((attrib & Material::CUBE_MAP) != 0U) {
        glDeleteTextures(1, &cube_map_tex);
        cube_map_tex = Material::loadCubeMapTexture(cube_map_tex_path);
    }
}

// Bind material
void Material::bind(const std::shared_ptr<GLSLProgram> &program) const {
    // Check the program
    if ((program == nullptr) || (!program->isValid())) {
        return;
    }

    // Use the program
    program->use();

    // Set color uniforms
    program->setUniform("ambient_color",      ambient_color);
    program->setUniform("diffuse_color",      diffuse_color);
    program->setUniform("specular_color",     specular_color);
    program->setUniform("transparency_color", transparency_color);

    // Set value uniforms
    program->setUniform("shininess",        shininess);
    program->setUniform("roughness",        roughness);
    program->setUniform("metalness",        metalness);
    program->setUniform("transparency",     transparency);
    program->setUniform("displacement",     displacement);
    program->setUniform("refractive_index", refractive_index);

    // Set texture uniforms
    program->setUniform("ambient_tex",      ambient_tex);
    program->setUniform("diffuse_tex",      diffuse_tex);
    program->setUniform("specular_tex",     specular_tex);
    program->setUniform("shininess_tex",    shininess_tex);
    program->setUniform("normal_tex",       normal_tex);
    program->setUniform("displacement_tex", displacement_tex);
    program->setUniform("cube_map_tex",     cube_map_tex);

    // Bind textures
    Material::bindTexture(0U, ambient_tex,      Material::white_tex);
    Material::bindTexture(1U, diffuse_tex,      Material::white_tex);
    Material::bindTexture(2U, specular_tex,     Material::white_tex);
    Material::bindTexture(3U, shininess_tex,    Material::white_tex);
    Material::bindTexture(4U, normal_tex,       Material::blue_tex);
    Material::bindTexture(5U, displacement_tex, Material::black_tex);
    Material::bindTexture(6U, cube_map_tex,     GL_FALSE);
}


// Destructor

// Material destructor
Material::~Material() {
    // Delete all textures
    glDeleteTextures(1, &ambient_tex);    
    glDeleteTextures(1, &diffuse_tex);    
    glDeleteTextures(1, &specular_tex);    
    glDeleteTextures(1, &shininess_tex);    
    glDeleteTextures(1, &normal_tex);    
    glDeleteTextures(1, &displacement_tex);    
    glDeleteTextures(1, &cube_map_tex);
}


// Public static methods

// Create the default textures
void Material::createDefaultTextures() {
    // Create the white texture
    if (Material::white_tex != GL_FALSE) {
        Material::white_tex = Material::createDefaultTexture(glm::vec3(1.0F));
    }

    // Create the black texture
    if (Material::black_tex != GL_FALSE) {
        Material::black_tex = Material::createDefaultTexture(glm::vec3(0.0F));
    }

    // Create the blue texture
    if (Material::blue_tex != GL_FALSE) {
        Material::blue_tex = Material::createDefaultTexture(glm::vec3(0.0F, 1.0F, 0.0F));
    }
}

// Delete the default textures
void Material::deleteDefaultTextures() {
    // Delete textures
    glDeleteTextures(1, &Material::white_tex);
    glDeleteTextures(1, &Material::black_tex);
    glDeleteTextures(1, &Material::blue_tex);

    // Reset textures
    Material::white_tex = GL_FALSE;
    Material::black_tex = GL_FALSE;
    Material::blue_tex = GL_FALSE;
}