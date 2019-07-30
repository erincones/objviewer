#include "scene.hpp"

#include <iostream>


// Private static attributes

// Instances counter
unsigned int Scene::instances = 0U;

// Glad loaded flag
bool Scene::initialized_glad = false;


// Static methods

// GLFW error callback
void Scene::error_callback(int error, const char *description) {
    std::cerr << "error " << error << ": " << description << std::endl;
}

// GLFW framebuffer size callback
void Scene::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // Get the scene and update the window resolution
    Scene *scene = static_cast<Scene *>(glfwGetWindowUserPointer(window));
    scene->window_width = width;
    scene->window_height = height;

    // Resize viewport
    glViewport(0, 0, width, height);
}


// Constructor

// Scene constructor
Scene::Scene(const std::string &title, const int &width, const int &height, const int &context_ver_maj, const int &context_ver_min) :
    // Defaults for window
    window(nullptr),
    window_title(title),
    window_width(width),
    window_height(height),
    
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

    // Count instance
    Scene::instances++;

    // Create window
    if (create_window) {
        // Set the window hints
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, context_ver_maj);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, context_ver_min);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create the scene window
        window = glfwCreateWindow(window_width, window_height, window_title.c_str(), nullptr, nullptr);
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

            // Setup the viewport and swap interval
            glViewport(0, 0, window_width, window_height);
            glfwSwapInterval(1);

            // Set the clear color
            glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0F);

            // Enable depth test
            glEnable(GL_DEPTH_TEST);
        }
    }
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


// Destructor

// Scene destructor
Scene::~Scene() {
    // Destroy window
    if (window != nullptr) {
        glfwDestroyWindow(window);
    }

    // Discount instance
    Scene::instances--;

    // Terminate GLFW if there are no instances
    if (Scene::instances == 0U) {
        glfwTerminate();
    }
}