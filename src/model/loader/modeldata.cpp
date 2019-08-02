#include "modeldata.hpp"


// Structs

// Object constructor
ModelData::Object::Object(const GLsizei &count, const GLsizei &offset, Material *const material) :
    count(count),
    offset(sizeof(GLsizei) * offset),
    material(material) {}


// Constructor

// Model data constructor
ModelData::ModelData(const std::string &path) :
    // Model file path
    model_path(path),

    // Open statuses
    model_open(false),
    material_open(false),

    // Geometry
    origin_mat(1.0F),
    min(INFINITY),
    max(-INFINITY),
    
    // Buffers
    vao(GL_FALSE),
    vbo(GL_FALSE),
    ebo(GL_FALSE),
    
    // Statistics
    vertices(0U),
    elements(0U),
    triangles(0U),
    textures(0U) {}


// Destructor

// Model data destructor
ModelData::~ModelData() {
    // Delete all objects
    for (const ModelData::Object *const object : object_stock) {
        delete object;
    }

    // Delete all materials
    for (const Material *const material : material_stock) {
        delete material;
    }
}