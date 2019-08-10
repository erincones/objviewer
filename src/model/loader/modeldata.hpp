#ifndef __MODEL_DATA_HPP_
#define __MODEL_DATA_HPP_

#include "../material.hpp"

#include "../../glad/glad.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <string>

#include <map>
#include <vector>


/** Model data abstract class */
class ModelData {
    public:
        // Structs

        /** Model object */
        struct Object {
            // Attributes

            /** Number of indices */
            GLsizei count;

            /** Index offset */
            GLsizei offset;

            /** Material */
            Material *material;


            // Constructor

            /** Object data constructor */
            Object(const GLsizei &count = 0, const GLsizei &offset = 0, Material *const material = nullptr);
        };


        // Attributes

        /** Model path */
        std::string model_path;

        /** Material path */
        std::string material_path;


        /** Model open status */
        bool model_open;

        /** Material open status */
        bool material_open;


        /** Origin matrix */
        glm::mat4 origin_mat;

        /** Minimum position values */
        glm::vec3 min;

        /** Maximum position values */
        glm::vec3 max;


        /** Vertex array object */
        GLuint vao;

        /** Vertex buffer object */
        GLuint vbo;

        /** Element buffer object */
        GLuint ebo;


        /** Object stock */
        std::vector<ModelData::Object *> object_stock;

        /** Material stock */
        std::vector<Material *> material_stock;


        /** Number of vertices */
        std::size_t vertices;

        /** Number of elements */
        std::size_t elements;

        /** Number of triangles */
        std::size_t triangles;

        /** Number of textures */
        std::size_t textures;


        // Constructors

        /** Disable the default constructor */
        ModelData() = delete;

        /** Model data constructor */
        ModelData(const std::string &path);


        // Destructor

        /** Model data destructor */
        virtual ~ModelData();
};

#endif // __MODEL_DATA_HPP_