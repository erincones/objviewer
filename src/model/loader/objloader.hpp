#ifndef __OBJ_LOADER_HPP_
#define __OBJ_LOADER_HPP_

#include "modelloader.hpp"

#include <string>


/** OBJ model format loader */
class OBJLoader : public ModelLoader {
    private:
        // Constructors

        /** Disable the default constructor */
        OBJLoader() = delete;

        /** Disable the default copy constructor */
        OBJLoader(const OBJLoader &) = delete;

        /** Disable the assignation operator */
        OBJLoader &operator=(const OBJLoader &) = delete;


        // Methods

        /** Parse, store the vertex and returns its index */
        GLsizei storeVertex(const std::string &vertex_str);

        /** Calculate the tangent vector for each vertex of a triangle */
        void calcTangent(const GLsizei &ind_0, const GLsizei &ind_1, const GLsizei &ind_2);

        /** Read data from file */
        bool read();

        /** Read material data from file */
        bool readMaterial(const std::string &mtl);


    public:
        // Constructor

        /** OBJ loader constructor */
        OBJLoader(const std::string &path);
};

#endif // __OBJ_LOADER_HPP_