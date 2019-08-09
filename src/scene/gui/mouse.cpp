#include "mouse.hpp"

#include <cmath>


// Private static const attributes

// Rotation angle
const float Mouse::ROTATION_ANGLE = 1.57079632679489661923F;


// Construction

// Mouse constructor
Mouse::Mouse(const int &width, const int &height) :
    // Pressed status
    pressed(false),

    // Window resolution
    width(width),
    height(height),

    // Points
    translation(0.0F),
    rotation(0.0F, 0.0F, 1.0F) {}


// Getters

// Get the pressed status
bool Mouse::isPressed() const {
    return pressed;
}


// Get the window resolution
glm::vec2 Mouse::getResolution() const {
    return glm::vec2(width, height);
}


// Get translation point
glm::vec2 Mouse::Mouse::getTranslationPoint() const {
    return translation;
}

// Get rotation point
glm::vec3 Mouse::getRotationPoint() const {
    return rotation;
}


// Setters

// Set the pressed status
void Mouse::setPressed(const bool &status) {
    pressed = status;
}


// Set the window resolution
void Mouse::setResolution(const int &new_width, const int &new_height) {
    width = static_cast<float>(new_width);
    height = static_cast<float>(new_height);
}


// Set the translation point
void Mouse::setTranslationPoint(const double &x, const double &y) {
    translation = normalizeToWindow(x, y);
}

// Set the rotation point
void Mouse::setRotationPoint(const double &x, const double &y) {
    rotation = projectToSphere(x, y);
}


// Methods

// Make a translation
glm::vec2 Mouse::translate(const double &x, const double &y) {
    // Calculate delta
    glm::vec2 point = normalizeToWindow(x, y);
    glm::vec2 delta = point - translation;

    // Update translation point
    translation = point;

    // Return delta
    return delta;
}

// Make a rotation
glm::quat Mouse::rotate(const double &x, const double &y) {
    // Calculate rotation
    glm::vec3 point = projectToSphere(x, y);
    glm::vec3 axis = glm::cross(rotation, point);
    glm::quat delta = glm::angleAxis(Mouse::ROTATION_ANGLE, axis);

    // Update rotation point
    rotation = point;

    // Return delta
    return glm::normalize(delta);
}


// Normalize a point
glm::vec2 Mouse::normalizeToWindow(const double &x, const double &y) {
    return glm::vec2((2.0F * static_cast<float>(x) - width) / width, (height - 2.0F * static_cast<float>(y)) / height);
}

// Project point to sphere
glm::vec3 Mouse::projectToSphere(const double &x, const double &y) {
    // Create sphere
    glm::vec2 normalized((2.0F * static_cast<float>(x) - width) / width, (height + 2.0F * static_cast<float>(y)) / height);
    glm::vec3 projection = glm::vec3(normalized, normalized.x * normalized.x + normalized.y * normalized.y);

    // Projecto to sphere or hyperbola
    projection.z = (projection.z <= 0.5F ? glm::sqrt(1.0F - projection.z) : 1.0F / (2.0F * std::sqrt(projection.z)));

    // Return projection
    return projection;
}