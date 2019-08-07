#include "material.hpp"

#define STBI_ASSERT(x)
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <iostream>


// Private static const attributes

// Texture attributes
const Material::Attribute Material::TEXTURE_ATTRIBUTE[] = {
    Material::AMBIENT,
    Material::DIFFUSE,
    Material::SPECULAR,
    Material::SHININESS,
    Material::NORMAL,
    Material::DISPLACEMENT
};


// Private static attributes

// Default texture
GLuint Material::default_texture[3] = {GL_FALSE, GL_FALSE, GL_FALSE};


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
void Material::bindTexture(const GLenum &index, const GLuint &texture) {
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, texture);
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
    color{glm::vec3(0.0F), glm::vec3(1.0F), glm::vec3(0.125F), glm::vec3(1.0F)},

    // Values
    value{10.0F, 0.3F, 0.1F, 0.0F, 0.05, 1.0F},

    // Textures
    texture{GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE},
    
    // Textures enabled status
    texture_enabled{true, true, true, true, true, true, true} {}


// Getters

// Get the material name
std::string Material::getName() const {
    return name;
}

// Get the color of the given attribute
glm::vec3 Material::getColor(const Material::Attribute &attrib) const {
    switch (attrib) {
        // Return color by attribute
        case Material::AMBIENT:      return color[0];
        case Material::DIFFUSE:      return color[1];
        case Material::SPECULAR:     return color[2];
        case Material::TRANSPARENCY: return color[3];

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
        case Material::SHININESS:        return value[0];
        case Material::ROUGHNESS:        return value[1];
        case Material::METALNESS:        return value[2];
        case Material::TRANSPARENCY:     return value[3];
        case Material::DISPLACEMENT:     return value[4];
        case Material::REFRACTIVE_INDEX: return value[5];

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
        case Material::AMBIENT:      return texture[0] == GL_FALSE ? Material::default_texture[2] : texture[0];
        case Material::DIFFUSE:      return texture[1] == GL_FALSE ? Material::default_texture[0] : texture[1];
        case Material::SPECULAR:     return texture[2] == GL_FALSE ? Material::default_texture[0] : texture[2];
        case Material::SHININESS:    return texture[3] == GL_FALSE ? Material::default_texture[0] : texture[3];
        case Material::NORMAL:       return texture[4] == GL_FALSE ? Material::default_texture[1] : texture[4];
        case Material::DISPLACEMENT: return texture[5] == GL_FALSE ? Material::default_texture[2] : texture[5];

        // Cubemap
        case Material::CUBE_MAP:
        case Material::CUBE_MAP_RIGHT: 
        case Material::CUBE_MAP_LEFT:
        case Material::CUBE_MAP_TOP:
        case Material::CUBE_MAP_BOTTOM:
        case Material::CUBE_MAP_FRONT:
        case Material::CUBE_MAP_BACK: return texture[6];

        // Invalid attribute
        default:
            std::cerr << "error: invalid attribute `" << attrib << "'" << std::endl;
            return GL_FALSE;
    }
}

// Get the texture enabled status
bool Material::isTextureEnabled(const Material::Attribute &attrib) const {
    switch (attrib) {
        // Return texture path by attribute
        case Material::AMBIENT:         return texture_enabled[0];
        case Material::DIFFUSE:         return texture_enabled[1];
        case Material::SPECULAR:        return texture_enabled[2];
        case Material::SHININESS:       return texture_enabled[3];
        case Material::NORMAL:          return texture_enabled[4];
        case Material::DISPLACEMENT:    return texture_enabled[5];

        // Cubemap
        case Material::CUBE_MAP:
        case Material::CUBE_MAP_RIGHT: 
        case Material::CUBE_MAP_LEFT:
        case Material::CUBE_MAP_TOP:
        case Material::CUBE_MAP_BOTTOM:
        case Material::CUBE_MAP_FRONT:
        case Material::CUBE_MAP_BACK: return texture_enabled[6];

        // Invalid attribute
        default:
            std::cerr << "error: invalid attribute `" << attrib << "'" << std::endl;
            return false;
    }
}

// Get the texture path of the given attribute
std::string Material::getTexturePath(const Material::Attribute &attrib) const {
    switch (attrib) {
        // Return texture path by attribute
        case Material::AMBIENT:         return texture_path[0];
        case Material::DIFFUSE:         return texture_path[1];
        case Material::SPECULAR:        return texture_path[2];
        case Material::SHININESS:       return texture_path[3];
        case Material::NORMAL:          return texture_path[4];
        case Material::DISPLACEMENT:    return texture_path[5];

        // Cube map texture paths
        case Material::CUBE_MAP_RIGHT:  return texture_path[ 6];
        case Material::CUBE_MAP_LEFT:   return texture_path[ 7];
        case Material::CUBE_MAP_TOP:    return texture_path[ 8];
        case Material::CUBE_MAP_BOTTOM: return texture_path[ 9];
        case Material::CUBE_MAP_FRONT:  return texture_path[10];
        case Material::CUBE_MAP_BACK:   return texture_path[11];

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
void Material::setColor(const Material::Attribute &attrib, const glm::vec3 &new_color) {
    switch (attrib) {
        // Set color by attribute
        case Material::AMBIENT:      color[0] = new_color; return;
        case Material::DIFFUSE:      color[1] = new_color; return;
        case Material::SPECULAR:     color[2] = new_color; return;
        case Material::TRANSPARENCY: color[3] = new_color; return;

        // Invalid attribute
        default:
            std::cerr << "error: invalid attribute `" << attrib << "'" << std::endl;
    }
}

// Set the value of the given attribute
void Material::setValue(const Material::Attribute &attrib, const float &new_value) {
    switch (attrib) {
        // Set value by attribute
        case Material::SHININESS:        value[0] = new_value; return;
        case Material::ROUGHNESS:        value[1] = new_value; return;
        case Material::METALNESS:        value[2] = new_value; return;
        case Material::TRANSPARENCY:     value[3] = new_value; return;
        case Material::DISPLACEMENT:     value[4] = new_value; return;
        case Material::REFRACTIVE_INDEX: value[5] = new_value; return;

        // Invalid attribute
        default:
            std::cerr << "error: invalid attribute `" << attrib << "'" << std::endl;
    }
}

// Get the texture enabled status
void Material::setTextureEnabled(const Material::Attribute &attrib, const bool &status) {
    switch (attrib) {
        // Return texture path by attribute
        case Material::AMBIENT:         texture_enabled[0] = status; return;
        case Material::DIFFUSE:         texture_enabled[1] = status; return;
        case Material::SPECULAR:        texture_enabled[2] = status; return;
        case Material::SHININESS:       texture_enabled[3] = status; return;
        case Material::NORMAL:          texture_enabled[4] = status; return;
        case Material::DISPLACEMENT:    texture_enabled[5] = status; return;

        // Cubemap
        case Material::CUBE_MAP:
        case Material::CUBE_MAP_RIGHT: 
        case Material::CUBE_MAP_LEFT:
        case Material::CUBE_MAP_TOP:
        case Material::CUBE_MAP_BOTTOM:
        case Material::CUBE_MAP_FRONT:
        case Material::CUBE_MAP_BACK: texture_enabled[6] = status; return;

        // Invalid attribute
        default:
            std::cerr << "error: invalid attribute `" << attrib << "'" << std::endl;
    }
}

// Set the texture path of the given attribute
void Material::setTexturePath(const Material::Attribute &attrib, const std::string &path) {
    switch (attrib) {
        // Set texture path by attribute
        case Material::AMBIENT:      texture_path[0] = path; break;
        case Material::DIFFUSE:      texture_path[1] = path; break;
        case Material::SPECULAR:     texture_path[2] = path; break;
        case Material::SHININESS:    texture_path[3] = path; break;
        case Material::NORMAL:       texture_path[4] = path; break;
        case Material::DISPLACEMENT: texture_path[5] = path; break;

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
            for (int i = 6; i < 12; i++) {
                texture_path[i] = path;
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
    for (int i = 6, j = 1; j < 6; j++) {
        texture_path[i] = path[j];
    }

    // Reload texture
    reloadTexture(Material::CUBE_MAP);
}


// Methods

// Reload textures
void Material::reloadTexture(const Material::Attribute &attrib) {
    // Reload textures
    for (int i = 0; i < 6; i++) {
        if (attrib & Material::TEXTURE_ATTRIBUTE[i]) {
            glDeleteTextures(1, &texture[i]);
            texture[i] = Material::load2DTexture(texture_path[i]);
        }
    }

    // Cube map texture
    if (attrib & Material::CUBE_MAP) {
        glDeleteTextures(1, &texture[6]);
        texture[6] = Material::loadCubeMapTexture({texture_path[6], texture_path[7], texture_path[8], texture_path[9], texture_path[10], texture_path[11]});
    }
}

// Bind material
void Material::bind(GLSLProgram *const program) const {
    // Check the program
    if ((program == nullptr) || (!program->isValid())) {
        return;
    }

    // Use the program
    program->use();

    // Set color uniforms
    program->setUniform("ambient_color",      color[0]);
    program->setUniform("diffuse_color",      color[1]);
    program->setUniform("specular_color",     color[2]);
    program->setUniform("transparency_color", color[3]);

    // Set value uniforms
    program->setUniform("shininess",        value[0]);
    program->setUniform("roughness",        value[1]);
    program->setUniform("metalness",        value[2]);
    program->setUniform("transparency",     value[3]);
    program->setUniform("displacement",     value[4]);
    program->setUniform("refractive_index", value[5]);

    // Set texture uniforms
    program->setUniform("ambient_tex",      0);
    program->setUniform("diffuse_tex",      1);
    program->setUniform("specular_tex",     2);
    program->setUniform("shininess_tex",    3);
    program->setUniform("normal_tex",       4);
    program->setUniform("displacement_tex", 5);
    program->setUniform("cube_map_tex",     6);

    // Bind textures
    Material::bindTexture(0U, (texture[0] == GL_FALSE) || !texture_enabled[0] ? Material::default_texture[2] : texture[0]);
    Material::bindTexture(1U, (texture[1] == GL_FALSE) || !texture_enabled[1] ? Material::default_texture[0] : texture[1]);
    Material::bindTexture(2U, (texture[2] == GL_FALSE) || !texture_enabled[2] ? Material::default_texture[0] : texture[2]);
    Material::bindTexture(3U, (texture[3] == GL_FALSE) || !texture_enabled[3] ? Material::default_texture[0] : texture[3]);
    Material::bindTexture(4U, (texture[4] == GL_FALSE) || !texture_enabled[4] ? Material::default_texture[1] : texture[4]);
    Material::bindTexture(5U, (texture[5] == GL_FALSE) || !texture_enabled[5] ? Material::default_texture[2] : texture[5]);
    Material::bindTexture(6U, texture[6]);
}


// Destructor

// Material destructor
Material::~Material() {
    // Delete all textures
    glDeleteTextures(6, &texture[0]);
}


// Public static methods

// Create the default textures
void Material::createDefaultTextures() {
    // Create the white texture
    if (Material::default_texture[0] == GL_FALSE) {
        Material::default_texture[0] = Material::createDefaultTexture(glm::vec3(1.0F));
    }

    // Create the blue texture
    if (Material::default_texture[1] == GL_FALSE) {
        Material::default_texture[1] = Material::createDefaultTexture(glm::vec3(0.0F, 0.0F, 1.0F));
    }

    // Create the black texture
    if (Material::default_texture[2] == GL_FALSE) {
        Material::default_texture[2] = Material::createDefaultTexture(glm::vec3(0.0F));
    }
}

// Delete the default textures
void Material::deleteDefaultTextures() {
    // Delete textures
    glDeleteTextures(3, &Material::default_texture[0]);

    // Reset textures
    Material::default_texture[0] = GL_FALSE;
    Material::default_texture[1] = GL_FALSE;
    Material::default_texture[2] = GL_FALSE;
}