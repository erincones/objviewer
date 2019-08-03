#include "interactivescene.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>


// Static const attributes

// Repository URL
char InteractiveScene::repository_url[] = "https://github.com/Rebaya17/objviewer-gcc/";


// Private statics methods

// GLFW framebuffer size callback
void InteractiveScene::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // Execute the scene framebuffer size callback
    Scene::framebuffer_size_callback(window, width, height);
}


// Private methods

// Draw the scene
void InteractiveScene::drawScene() {
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

// Draw the GUI
void InteractiveScene::drawGUI() {
    // Check the visibility of all windows
	if (!show_main_gui_win && !show_metrics_win && !show_about_win && !show_about_imgui_win) {
		return;
    }

	// New ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

    // Show the main GUI window
    if (show_main_gui_win) {
        showMainGUIWindow();
    }

    // Show the about window
    if (show_about_win) {
        InteractiveScene::showAboutWindow(show_about_win);
    }

	// Show the metrics built-in window
	if (show_metrics_win) {
        ImGui::ShowMetricsWindow(&show_metrics_win);
    }

    // Show the about ImGui built-in window
    if (show_about_imgui_win) {
        ImGui::ShowAboutWindow(&show_about_imgui_win);
    }

    // Render the GUI
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


// Draw the main window GUI
void InteractiveScene::showMainGUIWindow() {
    // Setup style
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
    ImGui::SetNextWindowPos(ImVec2(0.0F, 0.0F), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(470.0F, static_cast<float>(height)), ImGuiCond_Always);

    // Create the main GUI window
    const bool open = ImGui::Begin("Settings", &show_main_gui_win, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::PopStyleVar();

    // Abort if the window is not showing
    if (!show_main_gui_win || !open) {
        ImGui::End();
        return;
    }

    // About section
    if (ImGui::CollapsingHeader("User Guide", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Navigation mode
        ImGui::BulletText("ESCAPE to toggle the navigation mode.");
        ImGui::BulletText("Click in the scene to enter in the navigation mode.");
        ImGui::BulletText("F1 to toggle the about window.");
        ImGui::BulletText("F12 to toggle the Dear ImGui metrics window.");
        ImGui::BulletText("Double-click on title bar to collapse window.");

        // Windows manipulation
        ImGui::Spacing();
        if (ImGui::TreeNodeEx("Others than the settings window", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::BulletText("Click and drag on lower right corner to resize window\n(double-click to auto fit window to its contents).");
            ImGui::BulletText("Click and drag on any empty space to move window.");
            ImGui::BulletText("TAB/SHIFT+TAB to cycle through keyboard editable fields.");
            ImGui::BulletText("CTRL+Click on a slider or drag box to input value as text.");
            ImGui::TreePop();
        }

        // Keyboard input
        ImGui::Spacing();
        if (ImGui::TreeNodeEx("While editing text", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::BulletText("Hold SHIFT or use mouse to select text.");
            ImGui::BulletText("CTRL+Left/Right to word jump.");
            ImGui::BulletText("CTRL+A or double-click to select all.");
            ImGui::BulletText("CTRL+X,CTRL+C,CTRL+V to use clipboard.");
            ImGui::BulletText("CTRL+Z,CTRL+Y to undo/redo.");
            ImGui::BulletText("ESCAPE to revert.");
            ImGui::BulletText("You can apply arithmetic operators +,*,/ on numerical\nvalues. Use +- to subtract.");
            ImGui::TreePop();
        }

        // About and metrics buttons
        show_about_win       |= ImGui::Button("About OBJViewer");  ImGui::SameLine();
        show_about_imgui_win |= ImGui::Button("About Dear ImGui"); ImGui::SameLine();
        show_metrics_win     |= ImGui::Button("Metrics");
    }

    // Scene section
    if (ImGui::CollapsingHeader("Scene")) {
        // OpenGL info
        if (ImGui::TreeNodeEx("OpenGL", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Vendor: %s",       Scene::opengl_vendor);
            ImGui::Text("Renderer: %s",     Scene::opengl_renderer);
            ImGui::Text("Version: %s",      Scene::opengl_version);
            ImGui::Text("GLSL version: %s", Scene::glsl_version);
            ImGui::TreePop();
            ImGui::Separator();
        }

        // Window info
        if (ImGui::TreeNodeEx("Window", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Width:  %d",  width);
            ImGui::SameLine(210.0F);
            ImGui::Text("Seconds: %.3fs", glfwGetTime());
            ImGui::Text("Height: %d", height);
            ImGui::SameLine(210.0F);
            ImGui::Text("Frames:  %.3fE3", kframes);
            ImGui::Spacing();
            if (ImGui::ColorEdit3("Background", &clear_color.r)) {
                glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0F);
            }
            ImGui::TreePop();
            ImGui::Separator();
        }

        // Scene statistics
        if (ImGui::TreeNode("Statistics*")) {
            // Models statistics variables
            std::size_t vertices  = 0U;
            std::size_t elements  = 0U;
            std::size_t triangles = 0U;
            std::size_t materials = 0U;
            std::size_t textures  = 0U;

            // Calculate models statistics
            for (const std::pair<const std::size_t, const std::pair<const Model *const, const std::size_t> > &model_data : model_stock) {
                vertices  += model_data.second.first->getNumberOfVertices();
                elements  += model_data.second.first->getNumberOfElements();
                triangles += model_data.second.first->getNumberOfTriangles();
                materials += model_data.second.first->getNumberOfMaterials();
                textures  += model_data.second.first->getNumberOfTextures();
            }

            // Programs statistics variables
            std::size_t shaders = 0U;
            std::size_t default_shaders = Scene::default_program->getNumberOfShaders();

            // Calculate program statistics
            for (const std::pair<const std::size_t, const GLSLProgram *const> &program_data : program_stock) {
                shaders += program_data.second->getNumberOfShaders();
            }

            // Cameras
            ImGui::BulletText("Cameras: %lu", camera_stock.size());

            // Models
            if (ImGui::TreeNodeEx("modelstats", ImGuiTreeNodeFlags_DefaultOpen, "Models: %lu", model_stock.size())) {
                ImGui::Text("Elements:  %lu", elements); ImGui::HelpMarker("Total of vertices");
                ImGui::SameLine(210.0F);
                ImGui::Text("Materials: %lu", materials);
                ImGui::Text("Vertices:  %lu", vertices); ImGui::HelpMarker("Unique vertices");
                ImGui::SameLine(210.0F);
                ImGui::Text("Textures:  %lu", textures);
                ImGui::Text("Triangles: %lu", triangles);
                ImGui::TreePop();
            }

            // Programs
            if (ImGui::TreeNodeEx("programsstats", ImGuiTreeNodeFlags_DefaultOpen, "GLSL programs: %lu + 2", program_stock.size())) {
                ImGui::Text("Shaders: %lu + %lu", shaders, default_shaders); ImGui::HelpMarker("Loaded + Defaults");
                ImGui::TreePop();
            }

            // Note
            ImGui::Spacing();
            ImGui::Text("*Including the elements with errors.");

            // Pop statistics node
            ImGui::TreePop();
        }
    }

    // Cameras section
    if (ImGui::CollapsingHeader("Cameras")) {
        // Active camera
        ImGui::BulletText("Active");
        ImGui::Indent();
        cameraWidget(active_camera);
        ImGui::Unindent();
        ImGui::Spacing();

        // Camera indices
        std::size_t remove = 0U;

        // Draw each camera node
        for (const std::pair<const std::size_t, Camera *const> &camera_data : camera_stock) {
            // ID and title strings
            const std::string id = std::to_string(camera_data.first);
            const std::string camera_title = "Camera " + id;

            // Draw node and catch the selected to remove
            if (ImGui::TreeNode(id.c_str(), camera_title.c_str())) {
                if (!cameraWidget(camera_data.second, camera_data.first)) {
                    remove = camera_data.first;
                }
                ImGui::TreePop();
            }
        }

        // Remove camera
        if (remove != 0U) {
            removeCamera(remove);
        }

        // Add camera button
        ImGui::Spacing();
        if (ImGui::Button("Add camera", ImVec2(454.0F, 19.0F))) {
            addCamera();
        }
        ImGui::Spacing();
    }

    // End main window
    ImGui::End();
}


// Draw the camera widget
bool InteractiveScene::cameraWidget(Camera *const camera, const std::size_t &id) {
    // Keep camera flag
    bool keep = true;
    
    // For non active camera
    if (id != 0U) {
        // Select active button
        bool active = camera == active_camera;
        if (ImGui::Checkbox("Active", &active)) {
            active_camera = camera;
        }
        // Remove button
        if (camera_stock.size() > 1U) {
            keep = !ImGui::RemoveButton();
        }
        ImGui::Spacing();
    }

    // Projection
    bool orthogonal = camera->isOrthogonal();
    if (ImGui::RadioButton("Perspective", !orthogonal)) {
        camera->setOrthogonal(false);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Orthogonal", orthogonal)) {
        camera->setOrthogonal(true);
    }
    ImGui::SameLine(338.0F);
    ImGui::Text("Projection");
    
    // Position
    glm::vec3 value = camera->getPosition();
    if (ImGui::DragFloat3("Position", &value.x, 0.01F, 0.0F, 0.0F, "%.4F")) {
        camera->setPosition(value);
    }
    
    // Direction
    value = camera->getDirection();
    if (ImGui::DragFloat3("Direction", &value.x, 0.01F, 0.0F, 0.0F, "%.4F")) {
        camera->setDirection(value);
    }
    
    // Clipping planes
    glm::vec2 clipping = camera->getClipping();
    if (ImGui::DragFloat2("Clipping", &clipping.x, 0.01F, 0.0F, 0.0F, "%.4F")) {
        camera->setClipping(clipping);
    }
    ImGui::HelpMarker("(Near, Far)");

    // Field of view
    float fov = camera->getFOV();
    if (ImGui::DragFloat("FOV", &fov, 0.01F, 0.0F, 0.0F, "%.4F")) {
        camera->setFOV(fov);
    }

    // Separator for open nodes
    ImGui::Separator();

    return keep;
}

// Draw the model widget
bool InteractiveScene::modelWidget(std::pair<Model *const, std::size_t> &model_data, const std::size_t &id) {
    return true;
}

// Draw the program widget
bool InteractiveScene::programWidget(GLSLProgram *const program, const std::size_t &id) {
    return true;
}


// Constructor

// Interactive scene constructor
InteractiveScene::InteractiveScene(const std::string &title, const int &width, const int &height, const int &context_ver_maj, const int &context_ver_min) :
    // Scene
    Scene(title, width, height, context_ver_maj, context_ver_min),

    // Show default GUI windows
    show_main_gui_win(true),
    show_metrics_win(false),
    show_about_win(false),
    show_about_imgui_win(false),

    // Focus on GUI by default
    focus_gui(true) {
    // Load the GUI if is the first instance
    if ((Scene::instances == 1U) && Scene::initialized_glad) {
        // Set the user pointer to this scene and setup callbacks
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, InteractiveScene::framebuffer_size_callback);


        // Setup the ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        // Setup platform and render
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");

        // Setup key navigation and disable ini file
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.IniFilename = NULL;

        // Setup the global style
        ImGui::StyleColorsDark();
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0F);
        ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 2.0F);
    }
}


// Methods

/** Render main loop */
void InteractiveScene::mainLoop() {
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

        // Draw the scene and GUI
        drawScene();
        drawGUI();

        // Poll events and swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);

        // Count frame
        kframes += 0.001;
    }
}


// Destructor

// Interactive scene destructor
InteractiveScene::~InteractiveScene() {
    // Terminate GUI if is the last instance
    if ((Scene::instances == 1U) && Scene::initialized_glad) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}


// Public static methods

// Draw the about window GUI
void InteractiveScene::showAboutWindow(bool &show) {
    // Creates the about window and abort if is not showing
    if (!ImGui::Begin("About OBJViewer", &show, ImGuiWindowFlags_NoResize)) {
        ImGui::End();
        return;
    }

    // Title
    ImGui::Text("OBJViewer - Another OBJ models viewer");
    ImGui::Separator();

    // Signature
    ImGui::Text("By Erick Rincones 2019.");
    ImGui::Text("OBJViewer is licensed under the MIT License, see LICENSE for more information.");
    ImGui::Spacing();

    // Repository
    ImGui::Text("GitHub repository:"); ImGui::HelpMarker("Click to select all and press\nCTRL+V to copy to clipboard");

    ImGui::PushItemWidth(-1.0F);
    ImGui::InputText("repourl", InteractiveScene::repository_url, sizeof(InteractiveScene::repository_url), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_ReadOnly);
    ImGui::PopItemWidth();

    // End window
    ImGui::End();
}