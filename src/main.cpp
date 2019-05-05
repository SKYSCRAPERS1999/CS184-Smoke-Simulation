#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>
#include <math.h>
#include <GLFW/glfw3.h>
#include <nanogui/nanogui.h>

#include <OpenGL/gl.h>
#include <OpenGL/OpenGL.h>
#include <CGL/CGL.h>

#include "grid.h"
#include "common.h"
#include "callback.h"
#include "color.h"

using namespace nanogui;
using namespace std;

static std::random_device rd;

static mt19937 rng(rd()); // random number generator in C++11

bool debug = true;

Grid grid;
bool mouse_down = false;
bool is_pause = false;
bool shift_pressed = false;
int size_smoke = 5;
double amount_smoke = 50;
bool test = true;

GLFWwindow *window = nullptr;
Screen *screen = nullptr;
std::string strval = "A string";

// starts a smoke at a random location
void randomize_grid(Grid &grid, int num_speckle = 3, int size = 3) {
    uni_dis dis_x(0, NUMCOL - size); // uniform distribution in C++11
    uni_dis dis_y(0, NUMROW - size); // uniform distribution in C++11
    uni_dis dis_density(25, 75); // uniform distribution in C++11
    uni_dis dis_size(1, size);
    while (num_speckle--) {
        int chosen_x = dis_x(rng);
        int chosen_y = dis_y(rng);
        int chosen_size = dis_size(rng);
        double chosen_density = dis_density(rng);
        for (int i = 0; i < chosen_size; ++i) {
            for (int j = 0; j < chosen_size; ++j) {
                grid.setDensity(chosen_x + i, chosen_y + j,
                                grid.getDensity(chosen_x + i, chosen_y + j) + chosen_density);
            }
        }
    }
}

void Grid::display(int LIMIT = 3) {
    glClear(GL_COLOR_BUFFER_BIT);
    double width = 1 / (double) NUMCOL * 2;
    double height = 1 / (double) NUMROW * 2;
    for (int y = 0; y < NUMROW; ++y) {
        for (int x = 0; x < NUMCOL; ++x) {
            double density = this->getDensity(x, y);
            double temperature = this->getTemperature(x, y);
            if (density <= LIMIT) continue;

            // [0, 100] -> [360, 300]
            double hue = 360 - temperature * 0.6;
            // [0, 100]
            double saturate = 100.0;
            // [0, 100] -> [0, 100]
            double value = density;

            Vector3D rgb = hsv2rgb({hue, saturate, value});


            glColor3d(rgb.x, rgb.y, rgb.z);
//      glColor3d(density / 100, density / 100, density / 100);

            glBegin(GL_QUADS);
            double bottom_left_x = -1 + width * x;
            double bottom_left_y = -1 + height * y;

            glVertex2d(bottom_left_x, bottom_left_y);
            glVertex2d(bottom_left_x + width, bottom_left_y);
            glVertex2d(bottom_left_x + width, bottom_left_y + height);
            glVertex2d(bottom_left_x, bottom_left_y + height);
            glEnd();
        }
    }
    glEnd();
    glFlush();
}

int main() {
    grid = Grid(NUMCOL + 2, NUMROW + 2);

    // Parameters of smoke simulation. Allow for adjusting later.
    vector<Vector2D> external_forces;
    external_forces.resize(grid.width * grid.height, Vector2D(0.0, 0.0));

    // These parameters effect the smoke that gets placed down with mouse clicks
    double amount_temp = 50;
    double ambient_temperature = 0;
    glfwSetErrorCallback(error_callback);
    // Initialize
    if (!glfwInit()) {
        return -1;
    }
    glfwSetTime(0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Smoke Simulation", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);



    glGetError();
    if (debug) {
        glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    // Create a nanogui screen
    screen = new Screen();
    screen->initialize(window, true);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSwapInterval(1); // To prevent screen tearing
    glfwSwapBuffers(window);


    // Create nanogui GUI
    bool enabled = true;
    FormHelper *gui = new FormHelper(screen);
    nanogui::ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Form helper example");
    gui->addGroup("Basic type");
    gui->addVariable("string", strval);
    gui->addVariable("bool", test);
    screen->setVisible(true);
    screen->performLayout();
    nanoguiWindow->center();

    glfwSetKeyCallback(window, [](GLFWwindow *, int key, int scancode, int action, int mods) {
                           screen->keyCallbackEvent(key, scancode, action, mods);
                       }
    );

    glfwSetCharCallback(window, [](GLFWwindow *, unsigned int codepoint) {
        screen->charCallbackEvent(codepoint);
    });

    glfwSetDropCallback(window, [](GLFWwindow *, int count, const char **filenames) {
        screen->dropCallbackEvent(count, filenames);
    });

    glfwSetScrollCallback(window, [](GLFWwindow *, double x, double y) {
        screen->scrollCallbackEvent(x, y);
    });

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *, int width, int height) {
        screen->resizeCallbackEvent(width, height);
    });

#if defined(_OPENMP)
#pragma omp parallel
    {
        int rank, rankn;
        rank = omp_get_thread_num();
        rankn = omp_get_num_threads();
        printf("rank: %d / %d\n", rank, rankn);
    }
#endif
    // Callback functions
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, keyboard_callback);
    auto last_time = steady_clock::now();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        //bool to_print = ((rng() % 100) == 0) && (omp_get_thread_num() == 0);

        // Handle dragging of mouse to create a stream of smoke

        if (mouse_down) {
//            double xpos = grid.cursor_pos.x;
//            double ypos = grid.cursor_pos.y;
//
//            int row = int(NUMROW - NUMROW * ypos / double(WINDOW_HEIGHT));
//            int col = int(NUMCOL * xpos / double(WINDOW_WIDTH));
//
//            for (int y = row - size_smoke; y <= row + size_smoke; ++y) {
//                for (int x = col - size_smoke; x <= col + size_smoke; ++x) {
//                    double dis2 = pow(y - row, 2.0) + pow(x - col, 2.0);
//
//                    if (y < 1 || y >= grid.height - 1 || x < 1 || x >= grid.width - 1 ||
//                        (dis2 > size_smoke * size_smoke)) {
//                        continue;
//                    }
//
//                    // What type of function should fall off be?
//                    double fall_off = 2 * 1.0 / max(dis2, 1.0);
//
//                    double den = grid.getDensity(x, y);
//                    double temp = grid.getTemperature(x, y);
//                    grid.setDensity(x, y, min(den + amount_smoke * fall_off, 100.0));
//                    grid.setTemperature(x, y, min(temp + amount_temp * fall_off, 100.0));
//
//                }
//            }
            randomize_grid(grid);
            grid.printGrid();
        }
        auto cur_time = steady_clock::now();
        auto elapsed = duration_cast<milliseconds>(cur_time - last_time);

        if (!is_pause) {
            if (FREQ * elapsed.count() >= 1000) {
                last_time = cur_time;
                auto start_time = steady_clock::now();
                grid.simulate(1, external_forces, ambient_temperature);
                auto end_time = steady_clock::now();
                auto simulate_time = duration_cast<milliseconds>(end_time - start_time);
            } else {
            }
        }

        auto start_time = steady_clock::now();
        grid.display();
        auto end_time = steady_clock::now();
        auto display_time = duration_cast<milliseconds>(end_time - start_time);
//        if (to_print) {
//            printf("display_time = %lld mm\n", display_time.count());
//        }
        screen->drawContents();
        screen->drawWidgets();
        glfwSwapBuffers(window);

    }
    glfwTerminate();
    return 0;
}
