//
// Created by impulse on 2019-05-07.
//

#include <nanogui/nanogui.h>
#include <CGL/CGL.h>
#include "common.h"

using namespace nanogui;

extern int size_smoke;
extern double amount_smoke;
extern double amount_temperature;
extern double ambient_temperature;
extern Vector3D global_rgb;

class SmokeScreen : public nanogui::Screen {
public:

  SmokeScreen(GLFWwindow *window) {

    this->initialize(window, true);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Create nanogui GUI
    bool enabled = true;
    FormHelper *gui = new FormHelper(this);
    nanogui::ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Adjustable parameters");
    gui->addGroup("Smoke");
    gui->addVariable("Size(1 to 20)", size_smoke);
    gui->addVariable("Density(0 to 100)", amount_smoke);
    gui->addVariable("Heat(0 to 100)", amount_temperature);
    gui->addVariable("Ambient(0 to 100)", ambient_temperature);

    // Color Wheel
    TabWidget *tabWidget = this->add<TabWidget>();
    Widget *layer = tabWidget->createTab("Color Wheel");
    layer->setLayout(new GroupLayout());
    // Use overloaded variadic add to fill the tab widget with Different tabs.
    layer->add<Label>("Color wheel widget", "sans-bold");
    layer->add<ColorWheel>();
    layer = tabWidget->createTab("Function Graph");
    layer->setLayout(new GroupLayout());
    layer->add<Label>("Function graph widget", "sans-bold");

    layer = tabWidget->createTab("Function Graph");
    layer->setLayout(new GroupLayout());

    layer->add<Label>("Function graph widget", "sans-bold");

    Graph *graph = layer->add<Graph>("Some Function");

    graph->setHeader("E = 2.35e-3");
    graph->setFooter("Iteration 89");
    VectorXf &func = graph->values();
    func.resize(100);
    for (int i = 0; i < 100; ++i)
      func[i] = 0.5f * (0.5f * std::sin(i / 10.f) +
                        0.5f * std::cos(i / 23.f) + 1);

    // Dummy tab used to represent the last tab button.
    tabWidget->createTab("+");

    this->setVisible(true);
    this->performLayout();
    nanoguiWindow->center();
  }

private:

};