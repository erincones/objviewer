#include "light.hpp"

#include <glm/trigonometric.hpp>


// Constructors

// Light constructor
Light::Light(const Light::Type &type) :
    // Default statuses
    enabled(true),
    grabbed(false),

    // Type
    type(type),

    // Default spacial attributes
    direction(0.0F, 0.0F, 1.0F),
    position(0.0F, 0.0F, 2.0F),

    // Non directional default attributes
    attenuation(1.0F, 0.14F, 0.007F),
    cutoff(glm::radians(5.0F), glm::radians(7.5F)),

    // Default colors
    ambient_color(1.0F),
    diffuse_color(1.0F),
    specular_color(1.0F),

    // Default values
    ambient_level(1.0F),
    diffuse_level(1.0F),
    specular_level(1.0F),
    shininess(1.0F) {}


// Getters

// Get the enabled status
bool Light::isEnabled() const {
    return enabled;
}

// Get the grabbed status
bool Light::isGrabbed() const {
    return grabbed;
}


// Get the type
Light::Type Light::getType() const {
    return type;
}


// Get the direction
glm::vec3 Light::getDirection() const {
    return -direction;
}

// Get the position
glm::vec3 Light::getPosition() const {
    return position;
}

// Get attenuation
glm::vec3 Light::getAttenuation() const {
    return attenuation;
}

// Get cutoff
glm::vec2 Light::getCutoff() const {
    return glm::degrees(cutoff);
}


// Get the ambient color
glm::vec3 Light::getAmbientColor() const {
    return ambient_color;
}

// Get the diffuse color
glm::vec3 Light::getDiffuseColor() const {
    return diffuse_color;
}

// Get the specular color
glm::vec3 Light::getSpecularColor() const {
    return specular_color;
}


// Get the ambient level
float Light::getAmbientLevel() const {
    return ambient_level;
}

// Get the ambient level
float Light::getDiffuseLevel() const {
    return diffuse_level;
}

// Get the ambient level
float Light::getSpecularLevel() const {
    return specular_level;
}

// Get the shininess
float Light::getShininess() const {
    return shininess;
}


// Setters

// Set the enabled status
void Light::setEnabled(const bool &status) {
    enabled = status;
}

// Set the grabbed status
void Light::setGrabbed(const bool &status) {
    grabbed = status;
}


// Set the type
void Light::setType(const Light::Type &new_type) {
    type = new_type;
}


// Set the direction
void Light::setDirection(const glm::vec3 &new_direction) {
    direction = -glm::normalize(new_direction);
}

// Set the position
void Light::setPosition(const glm::vec3 &new_position) {
    position = new_position;
}

// Set attenuation
void Light::setAttenuation(const glm::vec3 &new_attenuation) {
    attenuation = new_attenuation;
}

// Set cutoff
void Light::setCutoff(const glm::vec2 &new_cutoff) {
    cutoff = glm::radians(new_cutoff);
}


// Set the ambient color
void Light::setAmbientColor(const glm::vec3 &color) {
    ambient_color = color;
}

// Set the diffuse color
void Light::setDiffuseColor(const glm::vec3 &color) {
    diffuse_color = color;
}

// Set the specular color
void Light::setSpecularColor(const glm::vec3 &color) {
    specular_color = color;
}


// Set the ambient level
void Light::setAmbientLevel(const float &value) {
    ambient_level = value;
}

// Set the ambient level
void Light::setDiffuseLevel(const float &value) {
    diffuse_level = value;
}

// Set the ambient level
void Light::setSpecularLevel(const float &value) {
    specular_level = value;
}

// Set the shininess
void Light::setShininess(const float &value) {
    shininess = value;
}


// Methods

// Bind the light
void Light::bind(GLSLProgram *const program) const {
    // Check the program
    if ((program == nullptr) || (!program->isValid())) {
        return;
    }

    // Use the program
    program->use();

    // Set uniforms for enabled light
    if (enabled) {
        program->setUniform("u_light_type", type);

        // Non point light
        if (type != Light::POINT) {
            program->setUniform("u_light_direction", direction);
        }

        // Non directional light
        if (type != Light::DIRECTIONAL) {
            program->setUniform("u_light_position",    position);
            program->setUniform("u_light_attenuation", attenuation);
            if (type == Light::SPOTLIGHT) {
                program->setUniform("u_light_cutoff", glm::cos(cutoff));
            }
        }

        program->setUniform("u_ambient",   ambient_level  * ambient_color);
        program->setUniform("u_diffuse",   diffuse_level  * diffuse_color);
        program->setUniform("u_specular",  specular_level * specular_color);
        program->setUniform("u_shininess", shininess);
    }

    // Levels to zero for non enabled light
    else {
        program->setUniform("u_light_type", Light::DIRECTIONAL);

        program->setUniform("u_ambient",  glm::vec3(0.0F));
        program->setUniform("u_diffuse",  glm::vec3(0.0F));
        program->setUniform("u_specular", glm::vec3(0.0F));
    }
}