#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

#include <cmath>


// Static attributes

// Boost flag
bool Camera::boosted = false;

// Normal speed
float Camera::speed = 0.5F;

// Boosted speed
float Camera::boosted_speed = 1.0F;


// Mouse sensibility
float Camera::sensibility = 15.0F;


// Zoom factor
float Camera::zoom_factor = 1.0625F;


// Private methods

// updateViewMatrix
void Camera::updateViewMatrix() {
    view_mat = glm::lookAt(position, position + front, glm::cross(right, front));
}

// Update projection matrices
void Camera::updateProjectionMatrices() {
    // Auxiliar values
    const float aspect = width / height;
    const float ratio = std::atan(fov / 2.0F);
    const float distance = glm::length(position);

    // Orthogonal limits
    const float y = ratio * distance;
    const float x = y * aspect;

    // Projection matrices
    orthogonal_mat = glm::ortho(-x, x, -y, y, near, far);
    perspective_mat = glm::perspective(fov, aspect, near, far);
}


// Constructor

// Camera constructor
Camera::Camera(const int &width, const int &height, const bool &orthogonal) :
    orthogonal(orthogonal),
    width(width),
    height(height) {
    // Load default values
    reset();
}


// Getters

// Get the orthogonal projection status
bool Camera::isOrthogonal() const {
    return orthogonal;
}


// Get position
glm::vec3 Camera::getPosition() const {
    return position;
}

// Get look direction
glm::vec3 Camera::getDirection() const {
    return front;
}

// Get look angles
glm::vec3 Camera::getRotation() const {
    return glm::degrees(glm::vec3(std::atan2(front.z, front.x), std::asin(front.y), std::asin(up.x)));
}

// Get up vector
glm::vec3 Camera::getUp() const {
    return up;
}


// Get field of view
float Camera::getFOV() const {
    return glm::degrees(fov);
}

// Get resolution
glm::vec2 Camera::getResolution() const {
    return glm::vec2(width, height);
}

// Get clipping
glm::vec2 Camera::getClipping() const {
    return glm::vec2(near, far);
}


// Get the view matrix
glm::mat4 Camera::getViewMatrix() const {
    return view_mat;
}

// Get the orthogonal projection matrix
glm::mat4 Camera::getOrthogonalMatrix() const {
    return orthogonal_mat;
}

// Get the perspective projection matrix
glm::mat4 Camera::getPerspectiveMatrix() const {
    return perspective_mat;
}

// Get the current projection matrix
glm::mat4 Camera::getProjectionMatrix() const {
    return orthogonal ? orthogonal_mat : perspective_mat;
}


// Setters

// Set the orthogonal projection status
void Camera::setOrthogonal(const bool &status) {
    orthogonal = status;
}


// Set position
void Camera::setPosition(const glm::vec3 &new_position) {
    position = new_position;
    updateViewMatrix();
    updateProjectionMatrices();
}

// Set look direction
void Camera::setDirection(const glm::vec3 &new_direction) {
    front = glm::normalize(new_direction);
    right = glm::cross(front, up);
    updateViewMatrix();
}

// Set look angles
void Camera::setRotation(const glm::vec3 &angle) {
    // Angles to radian
    glm::vec3 radian = glm::radians(glm::vec3(angle.x, glm::clamp(angle.y, -89.0F, 89.0F), angle.z));

    // Update the direction vector
    front.x = std::cos(radian.y) * std::cos(radian.x);
    front.y = std::sin(radian.y);
    front.z = std::cos(radian.y) * std::sin(radian.x);
    front = glm::normalize(front);

    // Update up world vector
    up.x = std::sin(radian.z);
    up.y = std::cos(radian.z);
    up = glm::normalize(up);

    // Update right vector
    right = glm::normalize(glm::cross(front, up));

    // Update view matrix
    updateViewMatrix();
}

// Set up vector
void Camera::setUp(const glm::vec3 &direction) {
    up = glm::normalize(direction);
    right = glm::normalize(glm::cross(front, up));
    updateViewMatrix();
}


// Set field of view
void Camera::setFOV(const float &new_fov) {
    fov = glm::radians(new_fov);
    updateProjectionMatrices();
}

// Set resolution
void Camera::setResolution(const glm::vec2 &resolution) {
    // Set the resolution values
    width =resolution.x;
    height = resolution.y;

    // Prevent divisions by zero
    if (height == 0.0F) {
        height = 1.0F;
    }

    // Update matrices
    updateProjectionMatrices();
}

// Set clipping
void Camera::setClipping(const glm::vec2 &clipping) {
    near = clipping.x;
    far = clipping.y;
    updateProjectionMatrices();
}


// Methods

// Reset to default values
void Camera::reset() {
    // View default values
    position = glm::vec3(0.0F, 0.0F,  2.0F);
    front    = glm::vec3(0.0F, 0.0F, -1.0F);
    right    = glm::vec3(1.0F, 0.0F,  0.0F);
    up       = glm::vec3(0.0F, 1.0F,  0.0F);

    // Projection default values
    fov  = glm::radians(30.0F);
    near = 0.01F;
    far  = 10.0F;

    // Rotation default values
    yaw   = -90.0F;
    pitch = 0.0F;

    // Update matrices
    updateViewMatrix();
    updateProjectionMatrices();
}


// Bind the camera
void Camera::bind(GLSLProgram *const program) const {
    // Check the program
    if ((program == nullptr) || (!program->isValid())) {
        return;
    }

    // Use the program
    program->use();

    // Set uniforms
    program->setUniform("up_dir",         up);
    program->setUniform("view_dir",       front);
    program->setUniform("view_pos",       position);
    program->setUniform("view_mat",       view_mat);
    program->setUniform("projection_mat", orthogonal ? orthogonal_mat : perspective_mat);
}


// Travell the camera
void Camera::travell(const Camera::Movement &direction, const double &time) {
    // Calculate distance
    float distance = (Camera::boosted ? Camera::boosted_speed : Camera::speed) * (float)time;

    // Move camera
    switch (direction) {
        case RIGHT: position += right * distance; break;
        case LEFT:  position -= right * distance; break;
        case UP:    position += up * distance; break;
        case DOWN:  position -= up * distance; break;
        case FRONT: position += glm::normalize(glm::cross(up, right)) * distance; break;
        case BACK:  position -= glm::normalize(glm::cross(up, right)) * distance; break;
    }

    // Update the view matrix
    updateViewMatrix();
}

// Translate the camera
void Camera::translate(const glm::vec3 &delta) {
    position += delta;
    updateViewMatrix();
}

// Apply zoom
void Camera::zoom(const double &direction) {
    fov = direction > 0 ? fov / Camera::zoom_factor : fov * Camera::zoom_factor;
    updateProjectionMatrices();
}


// Rotate the camera
void Camera::rotate(const glm::vec2 &delta) {
    // Calculate angles
    yaw += delta.x * Camera::sensibility;
    pitch = glm::clamp(pitch + delta.y * Camera::sensibility, -89.0F, 89.0F);

    // Update look vector
    front.x = std::cos(glm::radians(pitch)) * std::cos(glm::radians(yaw));
    front.y = std::sin(glm::radians(pitch));
    front.z = std::cos(glm::radians(pitch)) * std::sin(glm::radians(yaw));
    front  = glm::normalize(front);

    // Update right vector
    right = glm::normalize(glm::cross(front, up));

    // Update view matrix
    updateViewMatrix();
}


// Destructor

// Camera destructor
Camera::~Camera() {}


// Static getters

// Get the boost status
bool Camera::isBoosted() {
    return Camera::boosted;
}

// Get the normal speed
float Camera::getSpeed() {
    return Camera::speed;
}

// Get the boosted speed
float Camera::getBoostedSpeed() {
    return Camera::boosted_speed;
}


// Get mouse sensibility
float Camera::getSensibility() {
    return Camera::sensibility;
}


// Get the zoom factor
float Camera::getZoomFactor() {
    return Camera::zoom_factor;
}


// Static setters

// Set the boost status
void Camera::setBoosted(const bool &status) {
    Camera::boosted = status;
}

// Set the normal speed
void Camera::setSpeed(const float &speed) {
    Camera::speed = speed;
}

// Set the boosted speed
void Camera::setBoostedSpeed(const float &speed) {
    Camera::boosted_speed = speed;
}


// Set the mouse sensibility
void Camera::setSensibility(const float &sensibility) {
    Camera::sensibility = sensibility;
}


// Set the zoom factor
void Camera::setZoomFactor(const float &factor) {
    Camera::zoom_factor = factor;
}