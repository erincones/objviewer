#ifndef __MODEL_HPP_
#define __MODEL_HPP_

#include "loader/modelloader.hpp"
#include "loader/modeldata.hpp"
#include "material.hpp"

#include "../scene/glslprogram.hpp"

#include "../glad/glad.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

#include <string>

#include <vector>


/** 3D model */
class Model : private ModelData {
    private:
        // Attributes

        /** Position */
        glm::vec3 position;

        /** Rotation */
        glm::quat rotation;

        /** Dimension */
        glm::vec3 dimension;


        /** Model matrix */
        glm::mat4 model_mat;

        /** Origin matrix multiply by the model matrix */
        glm::mat4 model_origin_mat;

        /** Normal matrix */
        glm::mat3 normal_mat;


        // Constructors

        /** Disable the default copy constructor */
        Model(const Model &) = delete;

        /** Disable the assignation operator */
        Model &operator=(const Model &) = delete;


        // Methods

        /** Load the model from the model path */
        void load();

        /** Makes the model empty */
        void clear();

        /** Update model and normal matrices */
        void updateMatrices();


    public:
        // Constructor

        /** Empty model constructor */
        Model();

        /** Model constructor */
        Model(const std::string &path);


        // Getters

        /** Get the open status */
        bool isOpen() const;

        /** Get the open material status */
        bool isMaterialOpen() const;


        /** Get the model file path */
        std::string getPath() const;

        /** Get the material file path */
        std::string getMaterialPath() const;


        /** Get a material by index */
        Material *getMaterial(const std::size_t &index) const;


        /** Get the origin matrix */
        glm::mat4 getOriginMatrix() const;

        /** Get the model matrix */
        glm::mat4 getModelMatrix() const;

        /** Get the normal matrix */
        glm::mat4 getNormalMatrix() const;


        /** Get the position */
        glm::vec3 getPosition() const;

        /** Get the rotation quaternion */
        glm::quat getRotation() const;

        /** Get the rotation angles */
        glm::vec3 getRotationAngles() const;

        /** Get the scale */
        glm::vec3 getScale() const;

        /** Get the maximum position values */
        glm::vec3 getMax() const;

        /** Get the minimum position values */
        glm::vec3 getMin() const;


        /** Get the number of vertices */
        std::size_t getNumberOfVertices() const;

        /** Get the number of elemtns */
        std::size_t getNumberOfElements() const;

        /** Get the number of triangles */
        std::size_t getNumberOfTriangles() const;

        /** Get the number of materials */
        std::size_t getNumberOfMaterials() const;

        /** Get the number of textures */
        std::size_t getNumberOfTextures() const;


        // Setters

        /** Set the new path */
        void setPath(const std::string &new_path);


        /** Set the new position */
        void setPosition(const glm::vec3 &new_position);

        /** Set the new rotation angles in degrees */
        void setRotation(const glm::vec3 &new_rotation);

        /** Set the new rotation quaternion */
        void setRotation(const glm::quat &new_rotation);

        /** Set the new scale */
        void setScale(const glm::vec3 &new_scale);


        // Methods

        /** Reload model */
        void reload();

        /** Reload material */
        void reloadMaterial();

        /** Reset geometry */
        void resetGeometry();


        /** Draw the model */
        void draw(GLSLProgram *const program) const;


        /** Translate the model */
        void translate(const glm::vec3 &delta);

        /** Rotate certain degrees */
        void rotate(const glm::vec3 &delta);

        /** Rotate applying the quaternion */
        void rotate(const glm::quat &factor);

        /** Scale the given factor */
        void scale(const glm::vec3 &factor);


        // Destructor

        /** Model destructor */
        ~Model();
};

#endif // __MODEL_HPP_