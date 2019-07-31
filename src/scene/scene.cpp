#include "scene.hpp"

#include <iostream>


// Private static attributes

// Instances counter
unsigned int Scene::instances = 0U;

// Elements unique ID's
unsigned int Scene::element_id = 0U;

// Glad loaded flag
bool Scene::initialized_glad = false;


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
    for (const std::pair<const unsigned int, Camera *const> &camera_data : scene->camera_stock) {
        camera_data.second->setResolution(resolution);
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
    clear_color(0.45F, 0.55F, 0.60F) {
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
        current_camera = new Camera(width, height);
        camera_stock[Scene::element_id++] = current_camera;
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

            // Setup the swap interval
            glfwSwapInterval(1);

            // Set the clear color
            glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0F);

            // Enable depth test
            glEnable(GL_DEPTH_TEST);
        }
    }

    // Load default textures if there are no instances
    if (Scene::instances == 0U) {
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


// Get resolution
glm::vec2 Scene::getResolution() const {

}


// Get the current camera
Camera *Scene::getCurrentCamera() const {

}


// Get camera by ID
Camera *Scene::getCamera(const unsigned int &id) const {

}

// Get model by ID
Model *Scene::getModel(const unsigned int &id) const {

}

// Get program by ID
GLSLProgram *Scene::getProgram(const unsigned int &id) const {

}


// Setters

// Set title
void Scene::setTitle(const std::string &title) {

}


// Add camera
unsigned int Scene::addCamera() {

}

// Select current camara
bool Scene::selectCamera(const unsigned int &id) {

}


// Add empty model
unsigned int Scene::addModel() {

}

// Add model
unsigned int Scene::addModel(const std::string &path, const unsigned int &program_id) {

}


// Add empty GLSL program
unsigned int Scene::addProgram() {

}

// Add GLSL program without geometry shader
unsigned int Scene::addProgram(const std::string &vert, const std::string &frag) {

}

// Add GLSL program
unsigned int Scene::addProgram(const std::string &vert, const std::string &geom, const std::string &frag) {

}


// Methods

// Render main loop
void Scene::mainLoop() {
    // Check the window status
    if (window == nullptr) {
        std::cerr << "error: there is no window" << std::endl;
        return;
    }

    // The rendering main loop
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        // Clear color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        // Poll events and swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}


// Remove camera
bool Scene::removeCamera(const unsigned int &id) {

}

// Remove camera
bool Scene::removeModel(const unsigned int &id) {

}

// Remove program
bool Scene::removeProgram(const unsigned int &id) {

}


// Destructor

// Scene destructor
Scene::~Scene() {
    // Delete cameras
    for (const std::pair<const unsigned int, const Camera *const> &camera_data : camera_stock) {
        delete camera_data.second;
    }

    // Delete models
    for (const std::pair<const unsigned int, const Model *const> &model_data : model_stock) {
        delete model_data.second;
    }

    // Delete programs
    for (const std::pair<const unsigned int, const GLSLProgram *const> &program_data : program_stock) {
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


    // Discount instance
    Scene::instances--;

    // Terminate GLFW if there are no instances
    if (Scene::instances == 0U) {
        Material::deleteDefaultTextures();
        glfwTerminate();
    }
}


// Static getters

// Get the default program
GLSLProgram *Scene::getDefaultProgram() {
    return Scene::default_program;
}


// Static setters

// Set the default program without geometry shader
GLSLProgram *Scene::setDefaultProgram(const std::string &vert, const std::string &frag) {
    // Delete previous default program
    if (Scene::default_program != nullptr) {
        delete Scene::default_program;
    }

    // Create the new default program
    Scene::default_program = new GLSLProgram(vert, frag);
}

// Set the default program
GLSLProgram *Scene::setDefaultProgram(const std::string &vert, const std::string &geom, const std::string &frag) {
    // Delete previous default program
    if (Scene::default_program != nullptr) {
        delete Scene::default_program;
    }

    // Create the new default program
    Scene::default_program = new GLSLProgram(vert, geom, frag);
}

