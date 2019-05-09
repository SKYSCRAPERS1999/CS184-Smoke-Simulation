#include "common.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <thread>
#include <math.h>
//#include <GLFW/glfw3.h>
#include <nanogui/nanogui.h>
//#include <OpenGL/gl.h>
//#include <OpenGL/OpenGL.h>
#include <CGL/CGL.h>
#include "stb_image.h"

#include "grid.h"
#include "color.h"
#include "smoke_screen.h"
#include "shader.h"

using namespace nanogui;
using namespace std;

std::random_device rd;
mt19937 rng(rd()); // random number generator in C++11

Grid grid;
//bool mouse_down = false;
//bool is_pause = false;
//bool shift_pressed = false;
//bool is_modify_vf = false;
//bool reset = false;
//bool debug = true; // Set to true to record time
Vector2D enter_cell = Vector2D(0, 0);
Vector2D exit_cell = Vector2D(0, 0);

// Adjustable parameters for nanogui

int size_smoke = 3 * (Con::NUMROW / 100);
double amount_smoke = 90;
double amount_temperature = 50;
double ambient_temperature = 0;
double temperature_parameter = 0.015;
double smoke_density_parameter = 0.005;
double external_force_parameter = 0.5;
double num_iter = 16;

Vector3D global_rgb;
extern Vector3D picked_rgb;

static int size_mouse = 3 * (Con::NUMROW / 100);
static bool test = true;

GLFWwindow *window = nullptr;
Screen *screen = nullptr;

extern void set_callback(GLFWwindow* );
extern void error_callback(int error, const char* );

int main() {

    grid = Grid(Con::NUMCOL + 2, Con::NUMROW + 2);
    // Initialize the initial vector field for buoyant forces to point upwards (gravity)
    vector<Vector2D> external_forces(grid.width * grid.height, Vector2D(0, 0));

    // Initialize window and gui elements
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        return -1;
    }
//    glfwSetTime(0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    window = glfwCreateWindow(Con::WINDOW_WIDTH, Con::WINDOW_HEIGHT, "Smoke Simulation", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    build_shader_program();
    generate_vertices_array();
    // Create a nanogui screen
    screen = new SmokeScreen(window);
    set_callback(window);

#if defined(_OPENMP)
#pragma omp parallel
    {
        int rank, rankn;
        rank = omp_get_thread_num();
        rankn = omp_get_num_threads();
        printf("rank: %d / %d\n", rank, rankn);
    }
#endif

    glfwSwapInterval(1); // To prevent screen tearing
    glfwSwapBuffers(window);
    auto last_time = steady_clock::now();
    steady_clock::time_point rendering_start_time, rendering_end_time, simulation_start_time, simulation_end_time, cur_time;
    long long rendering_time, simulation_time;
    // Core while loop for simulation
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        // If the velocity field is reset, reset external forces
        if (Con::reset) {
            Con::reset = false;
            fill(external_forces.begin(), external_forces.end(), Vector2D(0, 0));
        }

        // If modifying the vector field, continuously update the current location of the mouse if not pressed
        if (Con::is_modify_vf && !Con::mouse_down) {
            double xpos = grid.cursor_pos.x;
            double ypos = grid.cursor_pos.y;

            int row = int(Con::NUMROW - Con::NUMROW * ypos / double(Con::WINDOW_HEIGHT));
            int col = int(Con::NUMCOL * xpos / double(Con::WINDOW_WIDTH));

            enter_cell = Vector2D(col, row);
        }

        // Handle dragging of mouse to create a stream of smoke or modifying the vector field
        if (Con::mouse_down) {
            if (Con::is_modify_vf) {
                double xpos = grid.cursor_pos.x;
                double ypos = grid.cursor_pos.y;

                int row = int(Con::NUMROW - Con::NUMROW * ypos / double(Con::WINDOW_HEIGHT));
                int col = int(Con::NUMCOL * xpos / double(Con::WINDOW_WIDTH));

                exit_cell = Vector2D(col, row);
                if (exit_cell.x != enter_cell.x || exit_cell.y != enter_cell.y) {
                    Vector2D direction_mouse_drag = exit_cell - enter_cell;
                    for (int y = row - size_mouse; y < row + size_mouse; y++) {
                        for (int x = col - size_mouse; x < col + size_mouse; x++) {
                            if (y < 1 || x < 1 || y >= grid.height - 1 || x >= grid.width - 1) {
                                continue;
                            }
                            external_forces[y * grid.width + x] = direction_mouse_drag.unit();
                        }
                    }
                    enter_cell = exit_cell;
                }
            } else {
                double xpos = grid.cursor_pos.x;
                double ypos = grid.cursor_pos.y;

                int row = int(Con::NUMROW - Con::NUMROW * ypos / double(Con::WINDOW_HEIGHT));
                int col = int(Con::NUMCOL * xpos / double(Con::WINDOW_WIDTH));

                for (int y = row - size_smoke; y <= row + size_smoke; ++y) {
                    for (int x = col - size_smoke; x <= col + size_smoke; ++x) {
                        double dis2 = pow(y - row, 2.0) + pow(x - col, 2.0);

                        if (y < 1 || y >= grid.height - 1 || x < 1 || x >= grid.width - 1 ||
                            (dis2 > size_smoke * size_smoke)) {
                            continue;
                        }


                        // What type of function should fall off be?
                        dis2 /= pow((Con::NUMCOL / 100.0), 2.0);
                        double fall_off = 2.0 / max(dis2, 1.0);

                        double den = grid.getDensity(x, y);
                        double temp = grid.getTemperature(x, y);
                        grid.setDensity(x, y, min(den + amount_smoke * fall_off, 100.0));
                        grid.setTemperature(x, y, min(temp + amount_temperature * fall_off, 100.0));

                    }
                }
            }
        }
        cur_time = steady_clock::now();
        auto elapsed = duration_cast<milliseconds>(cur_time - last_time);

        // Advance one step in the simulation
        if (Con::debug) simulation_start_time = steady_clock::now();
        if (!Con::is_pause && (Con::FREQ * elapsed.count() >= 1000)) {
            last_time = cur_time;
            grid.simulate(1, external_forces, ambient_temperature, temperature_parameter, smoke_density_parameter,
                          external_force_parameter, num_iter);
        }

        // Display the current state of the simulation
        if (Con::debug) {
            simulation_end_time = steady_clock::now();
            rendering_start_time = steady_clock::now();
        }

        // If modifying the vector field, display vector field. Otherwise, display the smoke.
        unsigned char data[Con::NUMROW * Con::NUMCOL * 3] = {0};
        if (Con::is_modify_vf) {
            for (int y = 0; y < Con::NUMROW; ++y) {
                for (int x = 0; x < Con::NUMCOL; ++x) {
                    Vector2D accumulated_direction = Vector2D(0.0, 0.0);
                    for (int ys = -1 + y; ys <= y + 1; ++ys) {
                        for (int xs = -1 + x; xs <= x + 1; ++xs) {
                            if (ys >= 0 && xs >= 0 && ys < Con::NUMROW && xs < Con::NUMCOL) {
                                accumulated_direction += external_forces[ys * grid.width + xs];
                            }
                        }
                    }
                    double hue = 0;
                    double saturate = 100;
                    double value = 100;
                    if (accumulated_direction.x == 0 && accumulated_direction.y == 0) {
                        value = 0;
                    } else {
                        accumulated_direction = accumulated_direction.unit();
                        double angle =
                                accumulated_direction.y >= 0 ? acos(accumulated_direction.x) :
                                acos(accumulated_direction.x) + PI;
                        angle = angle * 180 / PI;
                        hue = angle;
                    }

                    global_rgb = hsv2rgb({hue, saturate, value});

                    int index = y * Con::NUMCOL + x;
                    data[index * 3] = max(global_rgb.x * 255, 0.0);
                    data[index * 3 + 1] = max(global_rgb.y * 255, 0.0);
                    data[index * 3 + 2] = max(global_rgb.z * 255, 0.0);
                }
            }
        } else {
            for (int y = 0; y < Con::NUMROW; ++y) {
                for (int x = 0; x < Con::NUMCOL; ++x) {
                    double density = grid.getDensity(x, y);
                    if (density <= Con::DISPLAY_LIMIT) continue;
                    double temperature = grid.getTemperature(x, y);

                    double hue_center = 400;
                    double hue_halfspan = 50;
                    if (picked_rgb.norm() > Con::EPS) {
                        Vector3D picked_hsv = rgb2hsv(picked_rgb);
                        hue_center = picked_hsv.x;
                    }
                    // [0, 100] -> [450, 350]
                    double hue = (int) (hue_center - (temperature - hue_halfspan)) % 360;
                    // [0, 100]
                    double saturate = 100.0;
                    // [0, 100] -> [0, 100]
                    double value = density;

                    global_rgb = hsv2rgb({hue, saturate, value});

                    int index = y * Con::NUMCOL + x;
                    data[index * 3] = max(global_rgb.x * 255, 0.0);
                    data[index * 3 + 1] = max(global_rgb.y * 255, 0.0);
                    data[index * 3 + 2] = max(global_rgb.z * 255, 0.0);
                }
            }
        }
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Con::NUMCOL, Con::NUMROW, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        if (Con::debug) {
            rendering_end_time = steady_clock::now();
            rendering_time = duration_cast<milliseconds>(rendering_end_time - rendering_start_time).count();
            simulation_time = duration_cast<milliseconds>(simulation_end_time - simulation_start_time).count();
            if (rng() % 70 == 0) {
                printf("simulation_time = %lld mm\trendering_time = %lld mm\n", simulation_time, rendering_time);
            }
        }

        screen->drawContents();
        screen->drawWidgets();

        glfwSwapBuffers(window);

    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &texture);
    glfwTerminate();
    return 0;
}
