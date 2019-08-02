#include "glslprogram.hpp"

#include <iostream>
#include <fstream>


// Static attributes

/** Current program */
GLuint GLSLProgram::current_program = GL_FALSE;


// Private getters

// Get the location of the given uniform within the program
GLint GLSLProgram::getUniformLocation(const GLchar *&name) {
    // Return invalid location for invalid program
    if ((program == GL_FALSE) || (program != GLSLProgram::current_program)) {
        return -1;
    }

    // Return stored location
    std::map<std::string, GLint>::const_iterator result = location_stock.find(name);
    if (result != location_stock.end()) {
        return result->second;
    }

    // Query the non stored uniform location and insert in the location stock
    GLint location = glGetUniformLocation(program, name);
    location_stock[name] = location;

    // Return the uniform location
    return location;
}


// Private static methods

// Compile a shader from the given source path and type
GLuint GLSLProgram::compileShaderFile(const std::string &path, const GLenum &type) {
    // Empty source path
    if (path.empty()) {
        return GL_FALSE;
    }

    // Open the file and check it
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "error: cannot open the shader source file `" << path << "'" << std::endl;
        return GL_FALSE;
    }

    // Read the source code from file and close it
	const std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

    // Compile the shader
    GLuint shader = GLSLProgram::compileShaderSource(source.c_str(), type);
    if (shader == GL_FALSE) {
        std::cerr << "error: shader source path `" << path << "'" << std::endl;
    }

    // Return the shader
    return shader;
}

// Compile a shader with the given source and type
GLuint GLSLProgram::compileShaderSource(const GLchar *const &source, const GLenum &type) {
    // Create the new shader of the given type
    GLuint shader = glCreateShader(type);

    // Return if the shader could not be created
    if (shader == GL_FALSE) {
        // Print error message
        std::cerr << "error: could not create the shader object" << std::endl;

        // Return the non created shader
        return shader;
    }

    // Compile the shader
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // Check the compilation status
    GLint status = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    // Compilation error
    if (status == GL_FALSE) {
        // Get the log length
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        // Print the information log if is not empty
        if (length > 0) {
            // Get the information log data
            GLchar *log = new GLchar[length];
            glGetShaderInfoLog(shader, length, nullptr, log);
            
            // Print the information log data
            std::cerr << log;
            delete[] log;
        }

        // Print compilation error message
        std::cerr << "error: could not compile the shader source" << std::endl;

        // Delete the shader and reset it
        glDeleteShader(shader);
        shader = GL_FALSE;
    }

    // Return the shader
    return shader;
}


// Constructor

// Empty program constructor
GLSLProgram::GLSLProgram() :
    program(GL_FALSE),
    shaders(0U) {}

// GLSL program without geometry shader constructor
GLSLProgram::GLSLProgram(const std::string &vert, const std::string &frag) :
    // Program
    program(GL_FALSE),
    
    // Shaders source paths
    vert_path(vert),
    frag_path(frag),

    // Number of shaders
    shaders(0U) {
    // Link the program
    link();
}

// GLSL program constructor
GLSLProgram::GLSLProgram(const std::string &vert, const std::string &geom, const std::string &frag) :
    // Program
    program(GL_FALSE),
    
    // Shaders source paths
    vert_path(vert),
    geom_path(geom),
    frag_path(frag),

    // Number of shaders
    shaders(0U) {
    // Link the program
    link();
}


// Getters

// Get the validity status
bool GLSLProgram::isValid() const {
    return program != GL_FALSE;
}

// Get the program object
GLuint GLSLProgram::getProgramObject() const {
    return program;
}

// Get the path of a shader source of the program of the given type
std::string GLSLProgram::getShaderPath(const GLenum &type) const {
    switch (type) {
        // Shaders source paths
        case GL_VERTEX_SHADER:   return vert_path;
        case GL_GEOMETRY_SHADER: return geom_path;
        case GL_FRAGMENT_SHADER: return frag_path;
        
        // Non available shaders
        case GL_COMPUTE_SHADER:
            std::cerr << "error: the compute shader is not available for this GLSLProgram class implementation" << std::endl;
            return "NOT_AVAILABLE";

        case GL_TESS_CONTROL_SHADER:
        case GL_TESS_EVALUATION_SHADER:
            std::cerr << "error: the tessellation shader is not available for this GLSLProgram class implementation" << std::endl;
            return "NOT_AVAILABLE";

        // Invalid shader
        default:
            std::cerr << "error: invalid shader type (" << type << ")" << std::endl;
            return "INVALID";
    }
}


// Setters

// Set the value for an integer uniform
void GLSLProgram::setUniform(const GLchar *name, const GLint &value) {
    glUniform1i(getUniformLocation(name), value);
}

// Set the value for an unsigned integer uniform
void GLSLProgram::setUniform(const GLchar *name, const GLuint &value) {
    glUniform1ui(getUniformLocation(name), value);
}

// Set the value for a float uniform
void GLSLProgram::setUniform(const GLchar *name, const GLfloat &value) {
    glUniform1f(getUniformLocation(name), value);
}

// Set the value for a 2D vector uniform
void GLSLProgram::setUniform(const GLchar *name, const glm::vec2 &vector) {
    glUniform2fv(getUniformLocation(name), 1, &vector[0]);
}

// Set the value for a 3D vector uniform
void GLSLProgram::setUniform(const GLchar *name, const glm::vec3 &vector) {
    glUniform3fv(getUniformLocation(name), 1, &vector[0]);
}

// Set the value for a 4D vector uniform
void GLSLProgram::setUniform(const GLchar *name, const glm::vec4 &vector) {
    glUniform4fv(getUniformLocation(name), 1, &vector[0]);
}

// Set the value for a 3x3 matrix uniform
void GLSLProgram::setUniform(const GLchar *name, const glm::mat3 &matrix) {
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

// Set the value for a 4x4 matrix uniform
void GLSLProgram::setUniform(const GLchar *name, const glm::mat4 &matrix) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}


// Methods

// Link a new pogram using the current shaders source paths
void GLSLProgram::link() {
    // Delete previous program and reset ID
    if (program != GL_FALSE) {
        location_stock.clear();
        glDeleteProgram(program);
        program = GL_FALSE;
    }

    // Count not empty shaders and check mandatories
    shaders = 0;
    bool mandatory_empty = false;

    // Check vertex shader source path
    if (vert_path.empty()) {
        std::cerr << "error: the vertex shader source path cannot be empty" << std::endl;
        mandatory_empty = true;
    }
    else {
        shaders++;
    }

    // Check fragment shader source path
    if (vert_path.empty()) {
        std::cerr << "error: the fragment shader source path cannot be empty" << std::endl;
        mandatory_empty = true;
    }
    else {
        shaders++;
    }

    // Check geometry shader source path
    if (!geom_path.empty()) {
        shaders++;
    }

    // Check if mandatory shaders source paths are empty
    if (mandatory_empty) {
        return;
    }
    
    // Compile the vertex shader
    const GLuint vert = GLSLProgram::compileShaderFile(vert_path, GL_VERTEX_SHADER);
    if (vert == GL_FALSE) {
        return;
    }

    // Compile the fragment shader
    const GLuint frag = GLSLProgram::compileShaderFile(frag_path, GL_FRAGMENT_SHADER);
    if (frag == GL_FALSE) {
        glDeleteShader(vert);
        return;
    }

    // Compile the geometry shader
    const GLuint geom = GLSLProgram::compileShaderFile(geom_path, GL_GEOMETRY_SHADER);
    if (!geom_path.empty() && (geom == GL_FALSE)) {
        glDeleteShader(vert);
        glDeleteShader(frag);
        return;
    }
    
    // Create the new program object
    program = glCreateProgram();
    if (program == GL_FALSE) {
        std::cerr << "error: could not create the shader program object" << std::endl;

        // Delete shaders and return
        glDeleteShader(vert);
        glDeleteShader(frag);
        glDeleteShader(geom);
        return;
    }

    // Attach mandatory shaders
    glAttachShader(program, vert);
    glAttachShader(program, frag);

    // Attach geometry shader if is not empty
    if (geom != GL_FALSE) {
        glAttachShader(program, geom);
    }

    // Link program and delete attached shaders
    glLinkProgram(program);
    glDeleteShader(vert);
    glDeleteShader(frag);
    glDeleteShader(geom);

    // Check the link status
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    // Link error
    if (status == GL_FALSE) {
        // Get the log length
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        // Print the information log if is not empty
        if (length > 0) {
            // Get the information log data
            GLchar *log = new GLchar[length];
            glGetProgramInfoLog(program, length, nullptr, log);
            
            // Print the information log data
            std::cerr << log;
            delete[] log;
        }

        // Print compilation error message
        std::cerr << "error: could not link the shader program object" << std::endl;

        // Delete the program and reset it
        glDeleteProgram(program);
        program = GL_FALSE;
    }
}

// Link a new program using the given shaders source paths
void GLSLProgram::link(const std::string &vert, const std::string &frag) {
    // Clear the geometry source path
    geom_path.clear();

    // Update the new shaders source paths
    vert_path = vert;
    frag_path = frag;

    // Link a new program with the given shaders source paths
    link();
}

// Link the program with the given shaders source paths
void GLSLProgram::link(const std::string &vert, const std::string &geom, const std::string &frag) {
    // Update the new shaders source paths
    vert_path = vert;
    geom_path = geom;
    frag_path = frag;

    // Link a new program with the given shaders source paths
    link();
}

// Use the program
void GLSLProgram::use() const {
    // Update the current program if is not the current program
    if (program != GLSLProgram::current_program) {
        glUseProgram(program);
        GLSLProgram::current_program = program;
    }
}


// Destructor

// GLSL program destructor
GLSLProgram::~GLSLProgram() {
    if (program != GL_FALSE) {
        glDeleteProgram(program);
    }
}