#include "modelloader.hpp"

#include "objloader.hpp"

#include <iostream>


// Static attributes

// Space characters
const std::string ModelLoader::space = " \t\n\r\f\v";


// Structs

// Vertex data constructor
ModelLoader::Vertex::Vertex() :
    position(0.0F),
    uv_coord(0.0F),
    normal(0.0F),
    tangent(0.0F) {}


// Constructors

// Model loader constructor
ModelLoader::ModelLoader(const std::string &path) :
    model_data(new ModelData(path)) {}


// Private methods

// Load data to GPU
void ModelLoader::load() {
    // Vertex array object
    glGenVertexArrays(1, &model_data->vao);
    glBindVertexArray(model_data->vao);

    // Vertex buffer object
    glGenBuffers(1, &model_data->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, model_data->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ModelLoader::Vertex) * vertex_stock.size(), &vertex_stock[0], GL_STATIC_DRAW);

    // Element array buffer
    glGenBuffers(1, &model_data->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_data->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * index_stock.size(), &index_stock[0], GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ModelLoader::Vertex), reinterpret_cast<void *>(offsetof(ModelLoader::Vertex, position)));

    // Texture coordinate attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ModelLoader::Vertex), reinterpret_cast<void *>(offsetof(ModelLoader::Vertex, uv_coord)));

    // Normal attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ModelLoader::Vertex), reinterpret_cast<void *>(offsetof(ModelLoader::Vertex, normal)));

    // Tangent attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ModelLoader::Vertex), reinterpret_cast<void *>(offsetof(ModelLoader::Vertex, tangent)));

    // Unbind vertex array object
    glBindVertexArray(GL_FALSE);

    // Free memory
    vertex_stock.clear();
    index_stock.clear();
}


// Destructor

// Virtual model loader destructor
ModelLoader::~ModelLoader() {}


// Public static methods

// Read and load data
ModelData *ModelLoader::load(const std::string &path, const ModelLoader::Format &format) {
    // Create a null model loader
    ModelLoader *loader = nullptr;

    // Instanciate the loader
    switch (format) {
        case OBJ: loader = static_cast<ModelLoader *>(new OBJLoader(path)); break;

        // Return empty model data if the format is unknown
        default:
            std::cerr << "error: unknown model loader format `" << format << "'" << std::endl;
            return new ModelData(path);
    }

    // Read and load data
    if (loader->read()) {
        loader->load();
    }

    // Get the model data and delete loader
    ModelData *model_data = loader->model_data;
    delete loader;

    // Return the model data
    return model_data;
}

// Read and load the material data
std::vector<Material *> ModelLoader::loadMaterial(const std::string &path, const ModelLoader::Format &format) {
    // Create a null model loader
    ModelLoader *loader = nullptr;

    // Instanciate the loader
    switch (format) {
        case OBJ: loader = static_cast<ModelLoader *>(new OBJLoader(path)); break;

        // Return empty model data if the format is unknown
        default:
            std::cerr << "error: unknown model loader format `" << format << "'" << std::endl;
            return std::vector<Material *>();
    }

    // Read and load the material data
    loader->readMaterial(path);

    // Get the material data
    std::vector<Material *> material_stock(loader->model_data->material_stock);

    // Clean up the loader data
    loader->model_data->material_stock.clear();
    delete loader->model_data;
    delete loader;

    // Return the model data
    return material_stock;
}


// Right std::string trim
void ModelLoader::rtrim(std::string &str) {
    str.erase(str.find_last_not_of(ModelLoader::space) + 1);
}