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


// Help marker widget
void InteractiveScene::helpMarker(const char *const text) {
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(text);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
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
        current_camera->bind(program);

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
        InteractiveScene::showMainGUIWindow();
    }

    // Show the about window
    if (show_about_win) {
        InteractiveScene::showAboutWindow();
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
    const bool open = ImGui::Begin("OBJViewer Settings", &show_main_gui_win, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
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

    // End main window
    ImGui::End();
}

// Draw the about window GUI
void InteractiveScene::showAboutWindow() {
    // Creates the about window
    const bool open = ImGui::Begin("About OBJViewer", &show_about_win, ImGuiWindowFlags_NoResize);

    // Abort if the window is not showing
    if (!show_about_win || !open) {
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
    ImGui::Text("GitHub repository:"); InteractiveScene::helpMarker("Click to select all and press\nCTRL+V to copy to clipboard");

    ImGui::PushItemWidth(-1.0F);
    ImGui::InputText("###github", InteractiveScene::repository_url, sizeof(InteractiveScene::repository_url), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_ReadOnly);
    ImGui::PopItemWidth();

    // End window
    ImGui::End();
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