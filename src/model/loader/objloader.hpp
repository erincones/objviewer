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

    public:
        // Constructor

        /** OBJ loader constructor */
        OBJLoader(const std::string &path);


        // Methods

        /** Read data from file */
        bool read();
};

#endif // __OBJ_LOADER_HPP_