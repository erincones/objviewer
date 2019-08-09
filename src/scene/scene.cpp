#include "scene.hpp"

#include <iostream>

#define TEXTURE_BUFFERS 5


// Private static attributes

// Instances counter
std::size_t Scene::instances = 0U;

// Elements unique ID's
std::size_t Scene::element_id = 1U;

// Glad loaded flag
bool Scene::initialized_glad = false;


// Screen width
GLsizei Scene::screen_width = 0U;

// Screen
GLsizei Scene::screen_height = 0U;


/** Square vertex array object */
GLuint Scene::square_vao = GL_FALSE;

/** Square vertex buffer object */
GLuint Scene::square_vbo = GL_FALSE;


// Geometry frame buffer object
GLuint Scene::fbo = GL_FALSE;

// Render buffer object
GLuint Scene::rbo = GL_FALSE;

// Textures buffers
GLuint Scene::buffer_texture[TEXTURE_BUFFERS];


// OpenGL vendor
const GLubyte *Scene::opengl_vendor = nullptr;

// OpenGL renderer
const GLubyte *Scene::opengl_renderer = nullptr;

// OpenGL version
const GLubyte *Scene::opengl_version = nullptr;

// GLSL version
const GLubyte *Scene::glsl_version = nullptr;


// Private static methods

// Create the geometry frame buffer
void Scene::createGeometryFrameBuffer() {
    // Frame buffer object
    glGenFramebuffers(1, &Scene::fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, Scene::fbo);

    // Generate the texture buffers and attach each one
    glGenTextures(TEXTURE_BUFFERS, Scene::buffer_texture);

    // Position texture buffer
    Scene::attachTextureToFrameBuffer(0, GL_RGB16F, GL_RGB, GL_FLOAT);

    // Normal and displacement texture buffer
    Scene::attachTextureToFrameBuffer(1, GL_RGBA16F, GL_RGBA, GL_FLOAT);

    // Ambient texture buffer
    Scene::attachTextureToFrameBuffer(2, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);

    // Diffuse and alpha texture buffer
    Scene::attachTextureToFrameBuffer(3, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

    // Specular and shininess texture buffer
    Scene::attachTextureToFrameBuffer(4, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);


    // Color attachments to use
    GLenum attachment[TEXTURE_BUFFERS];
    for (GLuint i = 0; i < TEXTURE_BUFFERS; i++) {
        attachment[i] = GL_COLOR_ATTACHMENT0 + i;
    }

    // Set the list of draw buffers
    glDrawBuffers(TEXTURE_BUFFERS, attachment);


    // Render buffer object
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    // Set the render buffer storage
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Scene::screen_width, Scene::screen_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);


    // Check the geometry frame buffer creation
    const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "error: the geometry frame buffer object status is not complete (" << status << ")" <<  std::endl;
    }

    // Unbind geometry frame buffer object
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Create and attach texture to the frame buffer object
void Scene::attachTextureToFrameBuffer(const GLenum &attachment, const GLint &internalFormat, const GLenum &format, const GLenum &type) {
    // Bind texture
    glBindTexture(GL_TEXTURE_2D, Scene::buffer_texture[attachment]);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Texure data
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, Scene::screen_width, Scene::screen_height, 0, format, type, nullptr);

    // Attach texture buffer to the frame buffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, GL_TEXTURE_2D, Scene::buffer_texture[attachment], 0);
}


// Create a square fitted to the screen
void Scene::createSquare() {
    // Square data
    const float data[] = {
        // Vertices positions // Texture coordinates
        -1.0F,  1.0F, 0.0F,   0.0F, 1.0F,
        -1.0F, -1.0F, 0.0F,   0.0F, 0.0F,
         1.0F,  1.0F, 0.0F,   1.0F, 1.0F,
         1.0F, -1.0F, 0.0F,   1.0F, 0.0F
    };

    // Vertex array object
    glGenVertexArrays(1, &Scene::square_vao);
    glBindVertexArray(Scene::square_vao);

    // Vertex buffer object
    glGenBuffers(1, &Scene::square_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, Scene::square_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), &data, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(0));

    // Texture coordinate attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));

    // Unbind vertex array object
    glBindVertexArray(GL_FALSE);
}


// GLFW error callback
void Scene::errorCallback(int error, const char *description) {
    std::cerr << "error " << error << ": " << description << std::endl;
}

// GLFW framebuffer size callback
void Scene::framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    // Get the scene and update the window resolution
    Scene *const scene = static_cast<Scene *>(glfwGetWindowUserPointer(window));
    scene->width = width;
    scene->height = height;

    // Update all cameras resolution
    const glm::vec2 resolution(width, height);
    for (const std::pair<const std::size_t, Camera *const> &camera_data : scene->camera_stock) {
        camera_data.second->setResolution(resolution);
    }
}


// Private methods

// Draw the scene
void Scene::drawScene() {
    // Geometry pass

    // Program pointer
    GLSLProgram *program;

    // Bind the geometry frame buffer object
    glBindFramebuffer(GL_FRAMEBUFFER, Scene::fbo);

    // Clear color and depth buffers and resize the viewport
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, screen_width, screen_height);

    // Draw each model
    for (const std::pair<const std::size_t, std::pair<const Model *const, const std::size_t> > model_data : model_stock) {
        // Check the model status
        if (!model_data.second.first->isOpen()) {
            continue;
        }

        // Get the program
        std::map<std::size_t, std::pair<GLSLProgram *, std::string> >::const_iterator result = program_stock.find(model_data.second.second);
        program = (result == program_stock.end() ? program_stock[0U] : result->second).first;

        // Bind the camera
        active_camera->bind(program);

        // Draw the model
        model_data.second.first->draw(program);
    }


    // Lighting pass

    // Bind the default frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, GL_FALSE);

    // Clear color and depth buffers and resize the viewport
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);

    // Get the program and use it
    std::map<std::size_t, std::pair<GLSLProgram *, std::string> >::const_iterator result = program_stock.find(lighting_program);
    program = (result == program_stock.end() ? program_stock[1U] : result->second).first;

    // Set buffer texture uniforms
    program->use();
    program->setUniform("u_position_tex", 0);
    program->setUniform("u_normal_tex",   1);
    program->setUniform("u_ambient_tex",  2);
    program->setUniform("u_diffuse_tex",  3);
    program->setUniform("u_specular_tex", 4);

    // Bind buffer textures
    for (GLenum i = 0; i < TEXTURE_BUFFERS; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, Scene::buffer_texture[i]);
    }

    // Draw square
    glBindVertexArray(Scene::square_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Unbind square vertex array
    glBindVertexArray(GL_FALSE);
}


// Constructor

// Scene constructor
Scene::Scene(const std::string &title, const int &width, const int &height, const int &context_ver_maj, const int &context_ver_min) :
    // Defaults for window
    window(nullptr),
    title(title),
    width(width),
    height(height),

    // Clear color
    clear_color(0.45F, 0.55F, 0.60F),
    
    // Active camera
    active_camera(nullptr),

    // Geometry pass program ID
    lighting_program(1U) {
    // Create window flag
    bool create_window = true;

    // Initialize GLFW if there are no instances
    if (Scene::instances == 0U) {
        // Setup error callback
        glfwSetErrorCallback(Scene::errorCallback);

        // Initialize the library
        if (glfwInit() != GLFW_TRUE) {
            std::cerr << "error: cannot initialize GLFW" << std::endl;
            create_window = false;
        }
    }

    // Create window
    if (create_window) {
        // Set the window hints
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, context_ver_maj);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, context_ver_min);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create the scene window
        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

        // Create the default camera
        active_camera = new Camera(width, height);
        camera_stock[Scene::element_id++] = active_camera;
    }

    // Check the window creation
    if (window == nullptr) {
        std::cerr << "error: cannot create the application window" << std::endl;
    }

    // Setup the context
    else {
        // Set the user pointer to this scene and setup callbacks
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, Scene::framebufferSizeCallback);

        // Maximize window and setup as the current context
        glfwMaximizeWindow(window);
        glfwMakeContextCurrent(window);

        // Initialize Glad and check errors
        if (!Scene::initialized_glad && (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0)) {
            std::cerr << "error: cannot initialize glad" << std::endl;

            // Destroy window
            glfwDestroyWindow(window);
            window = nullptr;
        }

        // Finalize conext setup
        else {
            // Set true the initialized Glad flag
            Scene::initialized_glad = true;

            // Set the OpenGL strings
            Scene::opengl_vendor   = glGetString(GL_VENDOR);
            Scene::opengl_renderer = glGetString(GL_RENDERER);
            Scene::opengl_version  = glGetString(GL_VERSION);
            Scene::glsl_version    = glGetString(GL_SHADING_LANGUAGE_VERSION);

            // Setup the swap interval
            glfwSwapInterval(1);

            // Enable depth test
            glEnable(GL_DEPTH_TEST);

            // Set the clear color
            glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0F);

            // Create a empty default geometry pass program
            program_stock[0U] = std::pair<GLSLProgram *, std::string>(new GLSLProgram(), "Empty (Default geometry pass)");

            // Create a empty default lighting pass program
            program_stock[1U] = std::pair<GLSLProgram *, std::string>(new GLSLProgram(), "Empty (Default lighting pass)");
        }
    }

    // If there are no instances
    if ((Scene::instances == 0U) && Scene::initialized_glad) {
        // Get the screen resolution
        const GLFWvidmode *const video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        Scene::screen_width = video_mode->width;
        Scene::screen_height = video_mode->height;

        // Create the square to deferred shading
        Scene::createSquare();

        // Create the geometry frame buffer
        Scene::createGeometryFrameBuffer();

        // Load default textures
        Material::createDefaultTextures();
    }

    // Count instance
    Scene::instances++;
}


// Getters

// Get the valid status
bool Scene::isValid() {
    return window != nullptr;
}


// Get title
std::string Scene::getTitle() const {
    return title;
}

// Get resolution
glm::vec2 Scene::getResolution() const {
    return glm::vec2(width, height);
}


// Get the active camera
Camera *Scene::getActiveCamera() const {
    return active_camera;
}


// Get camera by ID
Camera *Scene::getCamera(const std::size_t &id) const {
    std::map<std::size_t, Camera *>::const_iterator result = camera_stock.find(id);
    return result == camera_stock.end() ? nullptr : result->second;
}

// Get model by ID
Model *Scene::getModel(const std::size_t &id) const {
    std::map<std::size_t, std::pair<Model *, std::size_t> >::const_iterator result = model_stock.find(id);
    return result == model_stock.end() ? nullptr : result->second.first;
}

// Get the program id of a model
std::size_t Scene::getModelProgram(const std::size_t &id) const {
    std::map<std::size_t, std::pair<Model *, std::size_t> >::const_iterator result = model_stock.find(id);
    return result == model_stock.end() ? 0U : result->second.second;
}

// Get program by ID
GLSLProgram *Scene::getProgram(const std::size_t &id) const {
    std::map<std::size_t, std::pair<GLSLProgram *, std::string> >::const_iterator result = program_stock.find(id);
    return result == program_stock.end() ? nullptr : result->second.first;
}

// Get program description by ID
std::string Scene::getProgramDescription(const std::size_t &id) const {
    std::map<std::size_t, std::pair<GLSLProgram *, std::string> >::const_iterator result = program_stock.find(id);
    return result == program_stock.end() ? "NOT_FOUND" : result->second.second;
}


// Get the lighting pass program ID
std::size_t Scene::getLightingPassProgramID() const {
    return lighting_program;
}


// Get the default geometry pass program
GLSLProgram *Scene::getDefaultGeometryPassProgram() {
    return program_stock[0U].first;
}

// Get the default geometry pass program description
std::string Scene::getDefaultGeometryPassProgramDescription() {
    return program_stock[0U].second;
}


// Get the default lighing pass program
GLSLProgram *Scene::getDefaultLightingPassProgram() {
    return program_stock[1U].first;
}

// Get the default lighing pass program description
std::string Scene::getDefaultLightingPassProgramDescription() {
    return program_stock[1U].second;
}


// Get frames
double Scene::getFrames() const {
    return kframes;
}


// Setters

// Select the active camara
bool Scene::selectCamera(const std::size_t &id) {
    // Find camera
    std::map<std::size_t, Camera *>::const_iterator result = camera_stock.find(id);
    
    // Return false if the camera does not exists
    if (result == camera_stock.end()) {
        return false;
    }

    // Select the new active camera
    active_camera = result->second;
    return true;
}

// Add camera
std::size_t Scene::addCamera(const bool &orthogonal) {
    camera_stock[Scene::element_id] = new Camera(width, height, orthogonal);
    return Scene::element_id++;
}


// Add empty model
std::size_t Scene::addModel() {
    model_stock[Scene::element_id] = std::pair<Model *, std::size_t>(new Model(), 0U);
    return Scene::element_id++;
}

// Add model
std::size_t Scene::addModel(const std::string &path, const std::size_t &program_id) {
    model_stock[Scene::element_id] = std::pair<Model *, std::size_t>(new Model(path), program_id);
    return Scene::element_id++;
}


// Add empty GLSL program
std::size_t Scene::addProgram(const std::string &desc) {
    program_stock[Scene::element_id] = std::pair<GLSLProgram *, std::string>(new GLSLProgram(), desc);
    return Scene::element_id++;
}

// Add GLSL program without geometry shader
std::size_t Scene::addProgram(const std::string &desc, const std::string &vert, const std::string &frag) {
    program_stock[Scene::element_id] = std::pair<GLSLProgram *, std::string>(new GLSLProgram(vert, frag), desc);
    return Scene::element_id++;
}

// Add GLSL program
std::size_t Scene::addProgram(const std::string &desc, const std::string &vert, const std::string &geom, const std::string &frag) {
    program_stock[Scene::element_id] = std::pair<GLSLProgram *, std::string>(new GLSLProgram(vert, geom, frag), desc);
    return Scene::element_id++;
}

// Set program description
bool Scene::setProgramDescription(const std::string &desc, const std::size_t &id) {
    // Find the program
    std::map<std::size_t, std::pair<GLSLProgram *, std::string> >::iterator result = program_stock.find(id);

    // Return false if the program does not exists
    if (result == program_stock.end()) {
        return false;
    }

    // Set the program description
    result->second.second = desc;
    return true;
}


// Get the lighting pass program
void Scene::setLightingPassProgram(const std::size_t &id) {
    lighting_program = id;
}


// Set the default geometry pass program
void Scene::setDefaultGeometryPassProgram(const std::string &desc, const std::string &vert, const std::string &frag) {
    program_stock[0U].first->link(vert, frag);
    program_stock[0U].second = desc + " (Default geometry pass)";
}

// Se the default geometry pass program description
void Scene::setDefaultGeometryPassProgramDescription(const std::string &desc) {
    program_stock[0U].second = desc;
}


// Set the default lighing pass program
void Scene::setDefaultLightingPassProgram(const std::string &desc, const std::string &vert, const std::string &frag) {
    program_stock[1U].first->link(vert, frag);
    program_stock[1U].second = desc + " (Default lighting pass)";
}

// Se the default lighing pass program description
void Scene::setDefaultLightingPassProgramDescription(const std::string &desc) {
    program_stock[1U].second = desc;
}


// Set title
void Scene::setTitle(const std::string &new_title) {
    title = new_title;
    glfwSetWindowTitle(window, title.c_str());
}

// Set program to model
std::size_t Scene::setProgramToModel(const std::size_t &program_id, const std::size_t &model_id) {
    // Search the model
    std::map<std::size_t, std::pair<Model *, std::size_t> >::iterator result = model_stock.find(model_id);

    // Return zero if the model does not exists
    if (result ==  model_stock.end()) {
        std::cerr << "error: could not found the model with id " << model_id << std::endl;
        return 0U;
    }

    // Get the previous program ID
    const std::size_t previous_program = result->second.second;

    // Set the program ID to the model
    result->second.second = program_id;

    // Return the previous program ID
    return previous_program;
}


// Methods

// Render main loop
void Scene::mainLoop() {
    // Check the window status
    if (window == nullptr) {
        std::cerr << "error: there is no window" << std::endl;
        return;
    }

    // Check the default geometry pass valid status
    if (!program_stock[0U].first->isValid()) {
        std::cerr << "warning: the default geometry pass program has not been set or is not valid" << std::endl;
    }

    // Check the default lighting pass valid status
    if (!program_stock[1U].first->isValid()) {
        std::cerr << "warning: the default lighting pass program has not been set or is not valid" << std::endl;
    }

    // The rendering main loop
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        // Draw the scene
        drawScene();

        // Poll events and swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);

        // Count frame
        kframes += 0.001;
    }
}


// Reload all programs
void Scene::reloadPrograms() {
    for (std::pair<const std::size_t, std::pair<GLSLProgram *, std::string> > &program_data : program_stock) {
        program_data.second.first->link();
    }
}


// Remove camera
bool Scene::removeCamera(const std::size_t &id) {
    // Check the camera stock size
    if (camera_stock.size() == 1U) {
        std::cerr << "error: the camera stock could not be empty" << std::endl;
        return false;
    }

    // Find the camera
    std::map<std::size_t, Camera *>::const_iterator result = camera_stock.find(id);

    // Return false if the camera does not exists
    if (result == camera_stock.end()) {
        return false;
    }

    // Update the current camera if is selected to delete
    if (result->second == active_camera) {
        active_camera = std::next(result, result->second == camera_stock.rbegin()->second ? -1 : 1)->second;
    }

    // Delete camera
    delete result->second;
    camera_stock.erase(result);

    return true;
}

// Remove camera
bool Scene::removeModel(const std::size_t &id) {
    // Search the model
    std::map<std::size_t, std::pair<Model *, std::size_t> >::const_iterator result = model_stock.find(id);

    // Return false if the model does not exists
    if (result == model_stock.end()) {
        return false;
    }

    // Delete the model
    delete result->second.first;
    model_stock.erase(result);

    return true;
}

// Remove program
bool Scene::removeProgram(const std::size_t &id) {
    // Check if is a default program ID
    if ((id == 0U) || (id == 1U)) {
        std::cerr << "error: cannot remove a default program" << std::endl;
        return false;
    }

    // Search the program
    std::map<std::size_t, std::pair<GLSLProgram *, std::string> >::const_iterator result = program_stock.find(id);

    // Return false if the program does not exists
    if (result == program_stock.end()) {
        return false;
    }

    // Delete the program
    delete result->second.first;
    program_stock.erase(result);

    return true;
}


// Remove the default geometry pass program
void Scene::removeDefaultGeometryPassProgram() {
    // Get the program data
    std::pair<GLSLProgram *, std::string> &program_data = program_stock[0U];

    // Delete the program
    delete program_data.first;

    // Create a empty program
    program_data.first =  new GLSLProgram();
    program_data.second = "Empty (Default geometry pass)";
}

// Remove the default lighting pass program
void Scene::removeDefaultLightingPassProgram() {
    // Get the program data
    std::pair<GLSLProgram *, std::string> &program_data = program_stock[1U];

    // Delete the program
    delete program_data.first;

    // Create a empty program
    program_data.first =  new GLSLProgram();
    program_data.second = "Empty (Default lighting pass)";
}


// Destructor

// Scene destructor
Scene::~Scene() {
    // Delete cameras
    for (const std::pair<const std::size_t, const Camera *const> &camera_data : camera_stock) {
        delete camera_data.second;
    }

    // Delete models
    for (const std::pair<const std::size_t, std::pair<const Model *const, std::size_t> > &model_data : model_stock) {
        delete model_data.second.first;
    }

    // Delete programs
    for (const std::pair<const std::size_t, std::pair<const GLSLProgram *const , const std::string> > &program_data : program_stock) {
        delete program_data.second.first;
    }

    // Destroy window
    if (window != nullptr) {
        glfwDestroyWindow(window);
    }

    // If is the last instance
    if ((Scene::instances == 1U) && Scene::initialized_glad) {
        // Delete the geometry frame buffer
        glDeleteTextures(TEXTURE_BUFFERS, Scene::buffer_texture);
        glDeleteRenderbuffers(1, &Scene::rbo);
        glDeleteFramebuffers(1, &Scene::fbo);

        // Delete the square vertex buffer object and vertex array object
        glDeleteBuffers(1, &Scene::square_vbo);
        glDeleteBuffers(1, &Scene::square_vao);

        // Delete the default tetures
        Material::deleteDefaultTextures();

        // Terminate GLFW
        glfwTerminate();

        // Reset initialized Glad flag
        Scene::initialized_glad = false;

        // Reset OpenGL strings
        Scene::opengl_vendor   = nullptr;
        Scene::opengl_renderer = nullptr;
        Scene::opengl_version  = nullptr;
        Scene::glsl_version    = nullptr;
    }

    // Discount instance
    Scene::instances--;
}


// Static getters

// Get the OpenGL vendor
const GLubyte *Scene::getOpenGLVendor() {
    return Scene::opengl_vendor;
}

// Get the OpenGL renderer
const GLubyte *Scene::getOpenGLRenderer() {
    return Scene::opengl_renderer;
}

// Get the OpenGL version
const GLubyte *Scene::getOpenGLVersion() {
    return Scene::opengl_version;
}

// Get the GLSL version
const GLubyte *Scene::getGLSLVersion() {
    return Scene::glsl_version;
}