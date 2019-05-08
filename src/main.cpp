#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>
#include <math.h>
//#include <GLFW/glfw3.h>
#include <nanogui/nanogui.h>
//#include <OpenGL/gl.h>
//#include <OpenGL/OpenGL.h>
#include <CGL/CGL.h>
#include "stb_image.h"

#include "grid.h"
#include "common.h"
#include "callback.h"
#include "color.h"
#include "smoke_screen.h"

using namespace nanogui;
using namespace std;

static std::random_device rd;

static mt19937 rng(rd()); // random number generator in C++11

Grid grid;
bool mouse_down = false;
bool is_pause = false;
bool shift_pressed = false;
bool is_modify_vf = false;
bool reset = false;
bool debug = true; // Set to true to record time
Vector2D enter_cell = Vector2D(0, 0);
Vector2D exit_cell = Vector2D(0, 0);
GLuint shader_program;

// Adjustable parameters for nanogui

int size_smoke = 3 * (NUMROW / 100);
double amount_smoke = 90;
double amount_temperature = 50;
double ambient_temperature = 0;
double temperature_parameter = 0.015;
double smoke_density_parameter = 0.005;
double external_force_parameter = 0.5;
double num_iter = 16;

Vector3D global_rgb;
extern Vector3D picked_rgb;

int size_mouse = 3 * (NUMROW / 100);

bool test = true;

// Vertex Array Object and Vertex Buffer Object
GLuint VBO, VAO, EBO, texture;

GLFWwindow *window = nullptr;
Screen *screen = nullptr;

// starts a smoke at a random location (deprecated)
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

void generate_vertices_array() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenTextures(1, &texture);
    GLuint elements[] = {
            0, 1, 2,
            2, 3, 0
    };
//    float vertices[] = {
//            //position & texturecoordinate
//            -1, 1, 0, 1, // top left
//            1, 1, 1, 1, // top right
//            1, -1, 1, 0,// bottom right
//            -1, -1, 0, 0, // bottom left
//    };
    float vertices[] = {
            // positions          // colors           // texture coords
            1, 1, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
            1, -1, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
            -1, -1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
            -1, 1, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
    };
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) (2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) (5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindTexture(GL_TEXTURE_2D, texture); // TODO change those methods
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_REPEAT);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
//    glEnableVertexAttribArray(0);

//    double width = 1 / (double) NUMCOL * 2;
//    double height = 1 / (double) NUMROW * 2;
//    for (int y = 0; y < NUMROW; ++y) {
//        for (int x = 0; x < NUMCOL; ++x) {
//            double bottom_left_x = -1 + width * x;
//            double bottom_left_y = -1 + height * y;
//
//            // setup first triangle
//            float rectangle_vertices[] = {
//                    bottom_left_x, bottom_left_y + height, // top left
//                    bottom_left_x + width, bottom_left_y + height, // top right
//                    bottom_left_x + width, bottom_left_y, // bottom right
////                    bottom_left_x + width, bottom_left_y, 0, // bottom right
//                    bottom_left_x, bottom_left_y, // bottom left
////                    bottom_left_x, bottom_left_y + height, 0, // top left
//            };
//            int index = y * NUMCOL + x;
//            glBindVertexArray(VAOs[index]);
//            glBindBuffer(GL_ARRAY_BUFFER, VBOs[index]);
//            glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), rectangle_vertices,
//                         GL_STATIC_DRAW); // TODO not sure if GL_DYNAMIC_DRAW is better
//            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
//            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
//            glEnableVertexAttribArray(0);
//        }
//    }
//    glBindVertexArray(0);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void set_callback() {
    glfwSetKeyCallback(window, [](GLFWwindow *, int key, int scancode, int action, int mods) {
        screen->keyCallbackEvent(key, scancode, action, mods);
    });

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
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, keyboard_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
}

void build_shader_program() {
    // vertex shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertex_shader);
    // check for compile errors
    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
    }
    // fragment shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragment_shader);
    // check for compile error
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
    }
    // link shaders
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glBindFragDataLocation(shader_program, 0, "outColor"); // TODO not sure
    glLinkProgram(shader_program);
    // check for link error
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
    }
    // delete shader
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}


int main() {
    grid = Grid(NUMCOL + 2, NUMROW + 2);

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

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Smoke Simulation", nullptr, nullptr);
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
    set_callback();

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
        if (reset) {
            reset = false;
            fill(external_forces.begin(), external_forces.end(), Vector2D(0, 0));
        }

        // If modifying the vector field, continuously update the current location of the mouse if not pressed
        if (is_modify_vf && !mouse_down) {
            double xpos = grid.cursor_pos.x;
            double ypos = grid.cursor_pos.y;

            int row = int(NUMROW - NUMROW * ypos / double(WINDOW_HEIGHT));
            int col = int(NUMCOL * xpos / double(WINDOW_WIDTH));

            enter_cell = Vector2D(col, row);
        }

        // Handle dragging of mouse to create a stream of smoke or modifying the vector field
        if (mouse_down) {
            if (is_modify_vf) {
                double xpos = grid.cursor_pos.x;
                double ypos = grid.cursor_pos.y;

                int row = int(NUMROW - NUMROW * ypos / double(WINDOW_HEIGHT));
                int col = int(NUMCOL * xpos / double(WINDOW_WIDTH));

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

                int row = int(NUMROW - NUMROW * ypos / double(WINDOW_HEIGHT));
                int col = int(NUMCOL * xpos / double(WINDOW_WIDTH));

                for (int y = row - size_smoke; y <= row + size_smoke; ++y) {
                    for (int x = col - size_smoke; x <= col + size_smoke; ++x) {
                        double dis2 = pow(y - row, 2.0) + pow(x - col, 2.0);

                        if (y < 1 || y >= grid.height - 1 || x < 1 || x >= grid.width - 1 ||
                            (dis2 > size_smoke * size_smoke)) {
                            continue;
                        }


                        // What type of function should fall off be?
                        //dis2 /= pow((NUMCOL / 100.0), 2.0);
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
        if (debug) simulation_start_time = steady_clock::now();
        if (!is_pause && (FREQ * elapsed.count() >= 1000)) {
            last_time = cur_time;
            grid.simulate(1, external_forces, ambient_temperature, temperature_parameter, smoke_density_parameter,
                          external_force_parameter, num_iter);
        }

        // Display the current state of the simulation
        if (debug) {
            simulation_end_time = steady_clock::now();
            rendering_start_time = steady_clock::now();
        }


        // If modifying the vector field, display vector field. Otherwise, display the smoke.
        unsigned char data[NUMROW * NUMCOL * 3];
        if (is_modify_vf) {
            // TODO uncomment
            for (int y = 0; y < NUMROW; ++y) {
                for (int x = 0; x < NUMCOL; ++x) {
                    Vector2D accumulated_direction = Vector2D(0.0, 0.0);
                    for (int ys = -1 + y; ys <= y + 1; ++ys) {
                        for (int xs = -1 + x; xs <= x + 1; ++xs) {
                            if (ys >= 0 && xs >= 0 && ys < NUMROW && xs < NUMCOL) {
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

                    int index = y * NUMCOL + x;
                    data[index * 3] = max(global_rgb.x * 255, 0.0);
                    data[index * 3 + 1] = max(global_rgb.y * 255, 0.0);
                    data[index * 3 + 2] = max(global_rgb.z * 255, 0.0);

//                    int vertexColorLocation = glGetUniformLocation(shader_program, "ourColor");
//                    glUniform4f(vertexColorLocation, global_rgb.x, global_rgb.y, global_rgb.z, 1.0f);
//                    glBindVertexArray(VAOs[index]);
//                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
////                    glDrawArrays(GL_TRIANGLES, 0, 6);
                }
            }
        } else {
            for (int y = 0; y < NUMROW; ++y) {
                for (int x = 0; x < NUMCOL; ++x) {
                    double density = grid.getDensity(x, y);
                    if (density <= DISPLAY_LIMIT) continue;
                    double temperature = grid.getTemperature(x, y);

                    double hue_center = 400;
                    double hue_halfspan = 50;
                    if (picked_rgb.norm() > EPS) {
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

                    int index = y * NUMCOL + x;
                    data[index * 3] = max(global_rgb.x * 255, 0.0);
                    data[index * 3 + 1] = max(global_rgb.y * 255, 0.0);
                    data[index * 3 + 2] = max(global_rgb.z * 255, 0.0);
                }
            }
        }
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, NUMCOL, NUMROW, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        if (debug) {
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
