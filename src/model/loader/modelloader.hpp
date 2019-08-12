#ifndef __MODEL_LOADER_HPP_
#define __MODEL_LOADER_HPP_

#include "modeldata.hpp"
#include "../material.hpp"

#include "../../glad/glad.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <map>
#include <vector>


/** Model loader abstract class */
class ModelLoader {
    protected:
        // Structs

        /** Model vertex */
        struct Vertex {
            public:
                // Attributes

                /** Vertex position */
                glm::vec3 position;

                /** Texture coordinate */
                glm::vec2 uv_coord;

                /** Normal vector */
                glm::vec3 normal;

                /** Tangent vector */
                glm::vec3 tangent;


                // Constructor

                /** Vertex data constructor */
                Vertex();
        };


        // Attributes

        /** Model data */
        ModelData *model_data;

        /** Position stock */
        std::vector<glm::vec3> position_stock;

        /** Texture coordinates stock */
        std::vector<glm::vec2> uv_coord_stock;

        /** Normal stock */
        std::vector<glm::vec3> normal_stock;


        /** Parsed vertices */
        std::map<std::string, GLsizei> parsed_vertex;

        /** Indices */
        std::vector<GLsizei> index_stock;

        /** Vertices */
        std::vector<Vertex> vertex_stock;


        // Constructors

        /** Model loader constructor */
        ModelLoader(const std::string &path);

        /** Disable the default constructor */
        ModelLoader() = delete;

        /** Disable the default copy constructor */
        ModelLoader(const ModelLoader &) = delete;

        /** Disable the assignation operator */
        ModelLoader &operator=(const ModelLoader &) = delete;


        // Methods

        /** Read data from file */
        virtual bool read() = 0;

        /** Read material data from file */
        virtual bool readMaterial(const std::string &path) = 0;

        /** Load data to GPU */
        void load();


        // Static attributes

        /** Space characters */
        static const std::string space;

    public:
        // Enumerations

        /** Formats */
        enum Format {
            OBJ
        };


        // Destructor

        /** Virtual model loader destructor */
        virtual ~ModelLoader();


        // Static methods

        /** Read and load data */
        static ModelData *load(const std::string &path, const ModelLoader::Format &format);

        /** Read and load the material data */
        static std::vector<Material *> loadMaterial(const std::string &path, const ModelLoader::Format &format);


        /** Right std::string trim */
        static void rtrim(std::string &str);
};

#endif // __MODEL_LOADER_HPP_