//
// Created by impulse on 2019-05-07.
//

#include <CGL/CGL.h>
#include "common.h"

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

    nanogui::ref<Slider> s_size_smoke = new nanogui::Slider(window);
    nanogui::ref<Slider> s_amount_smoke = new nanogui::Slider(window);
    nanogui::ref<Slider> s_amount_temperature = new nanogui::Slider(window);
    nanogui::ref<Slider> s_ambient_temperature = new nanogui::Slider(window);
    nanogui::ref<Slider> s_temperature_parameter = new nanogui::Slider(window);
    nanogui::ref<Slider> s_smoke_density_parameter = new nanogui::Slider(window);
    nanogui::ref<Slider> s_external_force_parameter = new nanogui::Slider(window);
    nanogui::ref<Slider> s_num_iter = new nanogui::Slider(window);

    s_size_smoke->setCallback([&](double ret) { Con::size_smoke = int(ret); });
    s_amount_smoke->setCallback([&](double ret) { Con::amount_smoke = ret; });
    s_amount_temperature->setCallback([&](double ret) { Con::amount_temperature = ret; });
    s_ambient_temperature->setCallback([&](double ret) { Con::ambient_temperature = ret; });
    s_temperature_parameter->setCallback([&](double ret) { Con::temperature_parameter = ret; });
    s_smoke_density_parameter->setCallback([&](double ret) { Con::smoke_density_parameter = ret; });
    s_external_force_parameter->setCallback([&](double ret) { Con::external_force_parameter = ret; });
    s_num_iter->setCallback([&](double ret) { Con::num_iter = ret; });

    s_size_smoke->setValue(Con::size_smoke);
    s_amount_smoke->setValue(Con::amount_smoke);
    s_amount_temperature->setValue(Con::amount_temperature);
    s_ambient_temperature->setValue(Con::ambient_temperature);
    s_temperature_parameter->setValue(Con::temperature_parameter);
    s_smoke_density_parameter->setValue(Con::smoke_density_parameter);
    s_external_force_parameter->setValue(Con::external_force_parameter);
    s_num_iter->setValue(Con::num_iter);

    s_size_smoke->setRange({3.0, 30.0});
    s_amount_smoke->setRange({50.0, 100.0});
    s_amount_temperature->setRange({0, 100.0});
    s_ambient_temperature->setRange({0, 50.0});
    s_temperature_parameter->setRange({0, 0.03});
    s_smoke_density_parameter->setRange({0, 0.01});
    s_external_force_parameter->setRange({0, 1.0});
    s_num_iter->setRange({4, 32});

    gui->addWidget("Size(0 to 30)", s_size_smoke);
    gui->addWidget("Density(50 to 100)", s_amount_smoke);
    gui->addWidget("Heat(0 to 100)", s_amount_temperature);
    gui->addWidget("Ambient(0 to 50)", s_ambient_temperature);
    gui->addWidget("Heat parameter(0 to 0.03)", s_temperature_parameter);
    gui->addWidget("Density parameter(0 to 0.01)", s_smoke_density_parameter);
    gui->addWidget("Force parameter(0 to 1.0)", s_external_force_parameter);
    gui->addWidget("Diffusion iterations(4 to 32)", s_num_iter);

    // Color Wheel
    nanogui::ref<TabWidget> tabWidget = this->add<TabWidget>();
    nanogui::ref<Widget> layer = tabWidget->createTab("Color Wheel");
    layer->setLayout(new GroupLayout(margin));

    // Use overloaded variadic add to fill the tab widget with Different tabs.
    layer->add<Label>("Color", "sans-bold");
    nanogui::ref<ColorWheel> color_wheel = layer->add<ColorWheel>();
    color_wheel->setCallback([&](const nanogui::Color &color) {
      Con::picked_rgb = Vector3D(color.r(), color.g(), color.b());
    });


    this->setVisible(true);
    this->performLayout();

    // set parameter window's postion to the right margin
    window->setPosition(window->position() - Vector2i(window->width(), 0));
  }

private:
  double margin = 10;
};


//    gui->addVariable("Size", Con::size_smoke);
//    gui->addVariable("Density(0 to 100)", Con::amount_smoke);
//    gui->addVariable("Heat(0 to 100)", Con::amount_temperature);
//    gui->addVariable("Ambient(0 to 100)", Con::ambient_temperature);
//    gui->addVariable("Heat parameter", Con::temperature_parameter);
//    gui->addVariable("Density parameter", Con::smoke_density_parameter);
//    gui->addVariable("Force parameter", Con::external_force_parameter);
//    gui->addVariable("Diffusion iterations", Con::num_iter);