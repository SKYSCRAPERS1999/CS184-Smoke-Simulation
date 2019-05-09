//
// Created by impulse on 2019-05-07.
//

#include <CGL/CGL.h>
#include "common.h"

extern Vector3D global_rgb;
Vector3D picked_rgb;

class SmokeScreen : public nanogui::Screen {
public:

  SmokeScreen(GLFWwindow *glfw_window) {

    using namespace nanogui;
    using namespace std;

    this->initialize(glfw_window, true);
    int width, height;
    glfwGetFramebufferSize(glfw_window, &width, &height);
    glViewport(0, 0, width, height);

    // Create nanogui GUI
    bool enabled = true;
    FormHelper *gui = new FormHelper(this);
    nanogui::ref<Window> window = gui->addWindow(Eigen::Vector2i(Con::WINDOW_WIDTH - margin, margin),
        "Adjustable parameters");

    gui->addGroup("Smoke");

    gui->addVariable("Size(1 to 20)", Con::size_smoke);
    gui->addVariable("Density(0 to 100)", Con::amount_smoke);
    gui->addVariable("Heat(0 to 100)", Con::amount_temperature);
    gui->addVariable("Ambient(0 to 100)", Con::ambient_temperature);
    gui->addVariable("Heat parameter", Con::temperature_parameter);
    gui->addVariable("Density parameter", Con::smoke_density_parameter);
    gui->addVariable("Force parameter", Con::external_force_parameter);
    gui->addVariable("Diffusion iterations", Con::num_iter);


    // Color Wheel
    nanogui::ref<TabWidget> tabWidget = this->add<TabWidget>();
    nanogui::ref<Widget> layer = tabWidget->createTab("Color Wheel");
    layer->setLayout(new GroupLayout(margin));

    // Use overloaded variadic add to fill the tab widget with Different tabs.
    layer->add<Label>("Color wheel widget", "sans-bold");
    nanogui::ref<ColorWheel> color_wheel = layer->add<ColorWheel>();
    color_wheel->setCallback([&](const nanogui::Color& color){
        picked_rgb = Vector3D(color.r(), color.g(), color.b());
    });


    this->setVisible(true);
    this->performLayout();

    // set parameter window's postion to the right margin
    window->setPosition(window->position() - Vector2i(window->width(), 0));
  }

private:
    double margin = 10;
};
