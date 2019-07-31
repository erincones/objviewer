#include "model.hpp"

#include "loader/modelloader.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>


// Private methods

// Load the model from the model path
void Model::load() {
    // Load the model
    ModelData *model_data = ModelLoader::load(model_path, ModelLoader::OBJ);

    // Seyt the open statuses
    model_open = model_data->model_open;
    material_open = model_data->material_open;

    // Set the material path
    material_path = model_data->material_path;

    // Set the geometry
    origin_mat = model_data->origin_mat;
    min = model_data->min;
    max = model_data->max;

    // Set the buffers
    vao = model_data->vao;
    vbo = model_data->vbo;
    ebo = model_data->ebo;

    // Set the stocks
    material_stock = model_data->material_stock;
    object_stock = model_data->object_stock;
    
    // Statistics
    vertices  = model_data->vertices;
    elements  = model_data->elements;
    triangles = model_data->triangles;
    textures  = model_data->textures;


    // Clea up the loader data
    model_data->object_stock.clear();
    model_data->material_stock.clear();
    delete model_data;
}

// Makes the model empty
void Model::clear() {
    // Open statuses
    model_open = false;
    material_open = false;

    // Material path
    material_path.clear();

    // Geometry
    origin_mat = glm::mat4(1.0F);
    min = glm::vec3(INFINITY);
    max = glm::vec3(-INFINITY);

    // Delete buffers
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &vao);

    // Reset buffers
    vao = GL_FALSE;
    vbo = GL_FALSE;
    ebo = GL_FALSE;
    
    // Statistics
    vertices  = 0U;
    elements  = 0U;
    triangles = 0U;
    textures  = 0U;

    // Clear stocks
    object_stock.clear();
    material_stock.clear();
}

// Update the model and normal matrices
void Model::updateMatrices() {
    // Get the transformation matrices
    const glm::mat4 identity = glm::mat4(1.0F);
    const glm::mat4 translation_mat = glm::translate(identity, position);
    const glm::mat4 rotation_mat    = glm::mat4_cast(rotation);
    const glm::mat4 scale_mat       = glm::scale(identity, dimension);

    // Update the model and normal matrix
    const glm::mat4 translation_rotation_mat = translation_mat * rotation_mat;
    model_mat = translation_rotation_mat * scale_mat;
    normal_mat = glm::inverse(glm::transpose(translation_rotation_mat));
}

// Constructor

// Empty model constructor
Model::Model() :
    ModelData(std::string()),
    
    // Geometry
    position(0.0F),
    dimension(1.0F),
    
    // Matrices
    model_mat(1.0F),
    normal_mat(1.0F) {}

// Model constructor
Model::Model(const std::string &path) :
    ModelData(path),
    
    // Geometry
    position(0.0F),
    dimension(1.0F),
    
    // Matrices
    model_mat(1.0F),
    normal_mat(1.0F) {
    // Load the model
    load();
}


// Getters

// Get the open status
bool Model::isOpen() const {
    return model_open;
}

// Get the open material status
bool Model::isMaterialOpen() const {
    return material_open;
}


// Get the model file path
std::string Model::getPath() const {
    return model_path;
}

// Get the material file path
std::string Model::getMaterialPath() const {
    return material_path;
}


// Get the material stock
std::vector<Material *> Model::getMaterialStock() const {
    return material_stock;
}

// Get a material by name
Material *Model::getMaterial(const std::string &material_name) const {
    // Find the material and return it
    for (Material *const material : material_stock) {
        if (material->getName() == material_name) {
            return material;
        }
    }

    // Return null if the material is not found
    std::cerr << "error: material `" << material_name << "' not found" << std::endl;
    return nullptr;
}


// Get the origin matrix
glm::mat4 Model::getOriginMatrix() const {
    return origin_mat;
}

// Get the model matrix
glm::mat4 Model::getModelMatrix() const {
    return model_mat;
}

// Get the normal matrix
glm::mat4 Model::getNormalMatrix() const {
    return normal_mat;
}


// Get the position
glm::vec3 Model::getPosition() const {
    return position;
}

// Get the rotation quaternion
glm::quat Model::getRotation() const {
    return rotation;
}

// Get the rotation angles
glm::vec3 Model::getRotationAngles() const {
    return glm::degrees(glm::eulerAngles(rotation));
}

// Get the scale
glm::vec3 Model::getScale() const {
    return dimension;
}

// Get the maximum position values
glm::vec3 Model::getMax() const {
    return max;
}

// Get the minimum position values
glm::vec3 Model::getMin() const {
    return min;
}


// Get the number of vertices
std::size_t Model::getNumberOfVertices() const {
    return vertices;
}

// Get the number of elemtns
std::size_t Model::getNumberOfElements() const {
    return elements;
}

// Get the number of triangles
std::size_t Model::getNumberOfTriangles() const {
    return triangles;
}

// Get the number of materials
std::size_t Model::getNumberOfMaterials() const {
    return material_stock.size();
}

// Get the number of textures
std::size_t Model::getNumberOfTextures() const {
    return textures;
}


// Setters

// Set the new path
void Model::setPath(const std::string &new_path) {
    model_path = new_path;
    reload();
}


// Set the new position
void Model::setPosition(const glm::vec3 &new_position) {
    position = new_position;
    updateMatrices();
}

// Set the new rotation angles in degrees
void Model::setRotation(const glm::vec3 &new_rotation) {
    rotation = glm::quat(glm::radians(new_rotation));
    updateMatrices();
}

// Set the new rotation quaternion
void Model::setRotation(const glm::quat &new_rotation) {
    rotation = new_rotation;
    updateMatrices();
}

// Set the new scale
void Model::setScale(const glm::vec3 &new_scale) {
    dimension = new_scale;
    updateMatrices();
}


// Methods

// Reload model
void Model::reload() {
    // Clear the model data
    clear();

    // Load if the path is not empty
    if (!model_path.empty()) {
        load();
    }
}

// Reload material
void Model::reloadMaterial() {

}

// Reset geometry
void Model::resetGeometry() {
    // Set the default values
    position = glm::vec3(0.0F);
    rotation = glm::quat();
    dimension = glm::vec3(1.0F);

    // Update matrices
    updateMatrices();
}


// Draw the model
void Model::draw(GLSLProgram *const program) const {
    // Check model and program status
    if (!model_open || (program == nullptr) || (!program->isValid())) {
        return;
    }

    // Use the program
    program->use();

    // Set model uniforms
    program->setUniform("model_mat", model_mat);
    program->setUniform("normal_mat", normal_mat);

    // Bind the vertex array object
    glBindVertexArray(vao);

    // Draw the objects
    for (const ModelData::Object *const object : object_stock) {
        // Bind material
        object->material->bind(program);

        // Draw triangles
        glDrawElements(GL_TRIANGLES, object->count, GL_UNSIGNED_INT, reinterpret_cast<void *>(object->offset));
    }

    // Unbind the vertex array object
    glBindVertexArray(GL_FALSE);
}


// Translate the model
void Model::translate(const glm::vec3 &delta) {
    position += delta;
    updateMatrices();
}

// Rotate certain degrees
void Model::rotate(const glm::vec3 &delta) {
    rotation = glm::normalize(glm::quat(glm::radians(delta)) * rotation);
    updateMatrices();
}

// Rotate applying the quaternion
void Model::rotate(const glm::quat &factor) {
    rotation = glm::normalize(factor * rotation);
    updateMatrices();
}

// Scale the given factor
void Model::scale(const glm::vec3 &factor) {
    // Scale
    dimension *= factor;

    // Fix not a number
    if (!std::isfinite(dimension.x)) dimension.x = 0.001F;
    if (!std::isfinite(dimension.y)) dimension.y = 0.001F;
    if (!std::isfinite(dimension.z)) dimension.z = 0.001F;

    // Update matrices
    updateMatrices();
}


// Destructor

// Model destructor
Model::~Model() {
    clear();
}