#include "scene.hpp"

#include <iostream>


// Private static attributes

// Instances counter
std::size_t Scene::instances = 0U;

// Elements unique ID's
std::size_t Scene::element_id = 1U;

// Glad loaded flag
bool Scene::initialized_glad = false;


// OpenGL vendor
const GLubyte *Scene::opengl_vendor = nullptr;

// OpenGL renderer
const GLubyte *Scene::opengl_renderer = nullptr;

// OpenGL version
const GLubyte *Scene::opengl_version = nullptr;

// GLSL version
const GLubyte *Scene::glsl_version = nullptr;


// Default program
GLSLProgram *Scene::default_program = nullptr;


// Static methods

// GLFW error callback
void Scene::error_callback(int error, const char *description) {
    std::cerr << "error " << error << ": " << description << std::endl;
}

// GLFW framebuffer size callback
void Scene::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // Resize viewport
    glViewport(0, 0, width, height);

    // Get the scene and update the window resolution
    Scene *scene = static_cast<Scene *>(glfwGetWindowUserPointer(window));
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
    for (const std::pair<const std::size_t, std::pair<const Model *const, const std::size_t> > model_data : model_stock) {
        // Check the model status
        if (!model_data.second.first->isOpen()) {
            continue;
        }

        // Check the program status
        std::map<std::size_t, GLSLProgram *>::const_iterator result = program_stock.find(model_data.second.second);
        GLSLProgram *const program = (result == program_stock.end() ? Scene::default_program : result->second);

        // Bind the camera
        active_camera->bind(program);

        // Draw the model
        model_data.second.first->draw(program);
    }
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
    active_camera(nullptr) {
    // Create window flag
    bool create_window = true;

    // Initialize GLFW if there are no instances
    if (Scene::instances == 0U) {
        // Setup error callback
        glfwSetErrorCallback(Scene::error_callback);

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
        glfwSetFramebufferSizeCallback(window, Scene::framebuffer_size_callback);

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

            // Set the clear color
            glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0F);

            // Enable depth test
            glEnable(GL_DEPTH_TEST);
        }
    }

    // Load default textures if there are no instances
    if ((Scene::instances == 0U) && Scene::initialized_glad) {
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

// Get program by ID
GLSLProgram *Scene::getProgram(const std::size_t &id) const {
    std::map<std::size_t, GLSLProgram *>::const_iterator result = program_stock.find(id);
    return result == program_stock.end() ? nullptr : result->second;
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
std::size_t Scene::addProgram() {
    program_stock[Scene::element_id] = new GLSLProgram();
    return Scene::element_id++;
}

// Add GLSL program without geometry shader
std::size_t Scene::addProgram(const std::string &vert, const std::string &frag) {
    program_stock[Scene::element_id] = new GLSLProgram(vert, frag);
    return Scene::element_id++;
}

// Add GLSL program
std::size_t Scene::addProgram(const std::string &vert, const std::string &geom, const std::string &frag) {
    program_stock[Scene::element_id] = new GLSLProgram(vert, geom, frag);
    return Scene::element_id++;
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

    // Check if the default program is not null
    if (Scene::default_program == nullptr) {
        std::cerr << "warning: the default program has not been set" << std::endl;
    }

    // The rendering main loop
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        // Clear color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw the scene
        drawScene();

        // Poll events and swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);

        // Count frame
        kframes += 0.001;
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
    // Search the program
    std::map<std::size_t, GLSLProgram *>::const_iterator result = program_stock.find(id);

    // Return false if the program does not exists
    if (result == program_stock.end()) {
        return false;
    }

    // Delete the program
    delete result->second;
    program_stock.erase(result);

    return true;
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
    for (const std::pair<const std::size_t, const GLSLProgram *const> &program_data : program_stock) {
        delete program_data.second;
    }

    // Delete the default program
    if (Scene::default_program != nullptr) {
        delete Scene::default_program;
    }


    // Destroy window
    if (window != nullptr) {
        glfwDestroyWindow(window);
    }

    // Delete textures and terminate GLFW if is the last instance
    if ((Scene::instances == 1U) && Scene::initialized_glad) {
        Material::deleteDefaultTextures();
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


// Get the default program
GLSLProgram *Scene::getDefaultProgram() {
    return Scene::default_program;
}


// Static setters

// Set the default program without geometry shader
void Scene::setDefaultProgram(const std::string &vert, const std::string &frag) {
    // Delete previous default program
    if (Scene::default_program != nullptr) {
        delete Scene::default_program;
    }

    // Create the new default program
    Scene::default_program = new GLSLProgram(vert, frag);
}

// Set the default program
void Scene::setDefaultProgram(const std::string &vert, const std::string &geom, const std::string &frag) {
    // Delete previous default program
    if (Scene::default_program != nullptr) {
        delete Scene::default_program;
    }

    // Create the new default program
    Scene::default_program = new GLSLProgram(vert, geom, frag);
}