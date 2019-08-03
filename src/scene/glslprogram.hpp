#ifndef __GLSL_PROGRAM_HPP_
#define __GLSL_PROGRAM_HPP_

#include "../glad/glad.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include <string>

#include <map>


/** GLSL program */
class GLSLProgram {
    private:
        // Attributes

        /** Program object */
        GLuint program;


        /** Vertex shader source path */
        std::string vert_path;

        /** Geometry shader source path */
        std::string geom_path;

        /** Fragment shader source path */
        std::string frag_path;


        /** Number of shaders */
        std::size_t shaders;


        /** Uniform location stock */
        std::map<std::string, GLint> location_stock;

        // Constructors

        /** Disable the default copy constructor */
        GLSLProgram(const GLSLProgram &) = delete;

        /** Disable the assignation operator */
        GLSLProgram &operator=(const GLSLProgram &) = delete;


        // Getters

        /** Get the location of the given uniform within the program */
        GLint getUniformLocation(const GLchar *&name);


        // Static attributes

        /** Current program */
        static GLuint current_program;


        // Static methods

        /** Compile a shader from the given source path and type */
        static GLuint compileShaderFile(const std::string &path, const GLenum &type);

        /** Compile a shader with the given source and type */
        static GLuint compileShaderSource(const GLchar *const &source, const GLenum &type);


    public:
        // Constructors

        /** Empty constructor */
        GLSLProgram();

        /** GLSL program without geometry shader constructor */
        GLSLProgram(const std::string &vert, const std::string &frag);

        /** GLSL program constructor */
        GLSLProgram(const std::string &vert, const std::string &geom, const std::string &frag);


        // Getters

        /** Get the validity status */
        bool isValid() const;

        /** Get the program object */
        GLuint getProgramObject() const;

        /** Get the path of a shader source of the program of the given type */
        std::string getShaderPath(const GLenum &type) const;


        /** Get the number of shaders */
        std::size_t getNumberOfShaders() const;


        // Setters

        /** Set the value for an integer uniform */
        void setUniform(const GLchar *name, const GLint &value);

        /** Set the value for an unsigned integer uniform */
        void setUniform(const GLchar *name, const GLuint &value);

        /** Set the value for a float uniform */
        void setUniform(const GLchar *name, const GLfloat &value);

        /** Set the value for a 2D vector uniform */
        void setUniform(const GLchar *name, const glm::vec2 &vector);

        /** Set the value for a 3D vector uniform */
        void setUniform(const GLchar *name, const glm::vec3 &vector);

        /** Set the value for a 4D vector uniform */
        void setUniform(const GLchar *name, const glm::vec4 &vector);

        /** Set the value for a 3x3 matrix uniform */
        void setUniform(const GLchar *name, const glm::mat3 &matrix);

        /** Set the value for a 4x4 matrix uniform */
        void setUniform(const GLchar *name, const glm::mat4 &matrix);


        // Methods

        /** Link a new pogram using the current shaders source paths */
        void link();

        /** Link a new program using the given shaders source paths */
        void link(const std::string &vert, const std::string &frag);

        /** Link a new program using the given shaders source paths */
        void link(const std::string &vert, const std::string &geom, const std::string &frag);

        /** Use the program */
        void use() const;


        // Destructor

        /** GLSL program destructor */
        virtual ~GLSLProgram();

};

#endif // __GLSL_PROGRAM_HPP_