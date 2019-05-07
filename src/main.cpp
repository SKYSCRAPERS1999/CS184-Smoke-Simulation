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

#include "grid.h"
#include "common.h"
#include "callback.h"
#include "color.h"

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
Vector2D enter_cell = Vector2D(0,0);
Vector2D exit_cell = Vector2D(0,0);

// Adjustable parameters for nanogui
int size_smoke = 5 * 2;
double amount_smoke = 90;
double amount_temperature = 50;
double ambient_temperature = 0;

int size_mouse = 3 * 2;

bool test = true;

// Vertex Array Object and Vertex Buffer Object
GLuint VBOs[NUMCOL * NUMROW], VAOs[NUMCOL * NUMROW], EBO[NUMCOL * NUMROW];

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
    GLuint elements[] = {
            0, 1, 2,
            2, 3, 0
    };
    double width = 1 / (double) NUMCOL * 2;
    double height = 1 / (double) NUMROW * 2;
    for (int y = 0; y < NUMROW; ++y) {
        for (int x = 0; x < NUMCOL; ++x) {
            double bottom_left_x = -1 + width * x;
            double bottom_left_y = -1 + height * y;

            // setup first triangle
            float rectangle_vertices[] = {
                    bottom_left_x, bottom_left_y + height, 0, // top left
                    bottom_left_x + width, bottom_left_y + height, 0, // top right
                    bottom_left_x + width, bottom_left_y, 0, // bottom right
//                    bottom_left_x + width, bottom_left_y, 0, // bottom right
                    bottom_left_x, bottom_left_y, 0, // bottom left
//                    bottom_left_x, bottom_left_y + height, 0, // top left
            };
            int index = y * NUMCOL + x;
            glBindVertexArray(VAOs[index]);
            glBindBuffer(GL_ARRAY_BUFFER, VBOs[index]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), rectangle_vertices,
                         GL_STATIC_DRAW); // TODO not sure if GL_DYNAMIC_DRAW is better
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[index]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(0);
        }
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
}

//void display(GLuint shader_program, int LIMIT = 3) {
//    for (int y = 0; y < NUMROW; ++y) {
//        for (int x = 0; x < NUMCOL; ++x) {
//            double density = grid.getDensity(x, y);
//            double temperature = grid.getTemperature(x, y);
//            if (density <= LIMIT) continue;
//
//            // [0, 100] -> [360, 300]
//            double hue = 360 - temperature * 0.6;
//            // [0, 100]
//            double saturate = 100.0;
//            // [0, 100] -> [0, 100]
//            double value = density;
//
//            Vector3D rgb = hsv2rgb({hue, saturate, value});
//
//            int index = (y * NUMCOL + x) * 2;
//            glUseProgram(shader_program);
//            int vertexColorLocation = glGetUniformLocation(shader_program, "ourColor");
//            glUniform4f(vertexColorLocation, 0.0f, 0.5f, 0.0f, 1.0f);
//            glBindVertexArray(VAOs[index]);
//            glDrawArrays(GL_TRIANGLES, 0, 3);
//            glUniform4f(vertexColorLocation, 0.5f, 0.0f, 0.0f, 1.0f);
//            glBindVertexArray(VAOs[index + 1]);
//            glDrawArrays(GL_TRIANGLES, 0, 3);
//        }
//    }
//}

GLuint build_shader_program() {
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
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
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

    return shader_program;
}

int main() {
    grid = Grid(NUMCOL + 2, NUMROW + 2);

    // Initialize the initial vector field for buoyant forces to point upwards (gravity)
    vector<Vector2D> external_forces(grid.width*grid.height, Vector2D(0, 0));
    
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

    GLuint shader_program = build_shader_program();
    glGenVertexArrays(NUMCOL * NUMROW, VAOs);
    glGenBuffers(NUMCOL * NUMROW, VBOs);
    glGenBuffers(NUMCOL * NUMROW, EBO);
    generate_vertices_array();
    // Create a nanogui screen
    screen = new Screen();
    screen->initialize(window, true);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Create nanogui GUI
    bool enabled = true;
    FormHelper *gui = new FormHelper(screen);
    nanogui::ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Adjustable parameters");
    gui->addGroup("Smoke");
    gui->addVariable("Size(1 to 20)", size_smoke);
    gui->addVariable("Density(0 to 100)", amount_smoke);
    gui->addVariable("Heat(0 to 100)", amount_temperature);
    gui->addVariable("Ambient(0 to 100)", ambient_temperature);

    screen->setVisible(true);
    screen->performLayout();
    nanoguiWindow->center();

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
                            external_forces[y*grid.width + x] = direction_mouse_drag.unit();
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
                        dis2 /= pow((NUMCOL / 100.0), 2.0);
                        double fall_off = 2.0 / max(dis2, 1.0);

                        double den = grid.getDensity(x, y);
                        double temp = grid.getTemperature(x, y);
                        grid.setDensity(x, y, min(den + amount_smoke * fall_off, 100.0));
                        grid.setTemperature(x, y, min(temp + amount_temperature * fall_off, 100.0));

                    }
                }
            }
        }
        auto cur_time = steady_clock::now();
        auto elapsed = duration_cast<milliseconds>(cur_time - last_time);
        
        // Advance one step in the simulation
        if (!is_pause) {
            if (FREQ * elapsed.count() >= 1000) {
                last_time = cur_time;
                auto start_time = steady_clock::now();
                grid.simulate(1, external_forces, ambient_temperature);
                auto end_time = steady_clock::now();
                auto simulate_time = duration_cast<milliseconds>(end_time - start_time);
            }
        }

        // Display the current state of the simulation
        auto start_time = steady_clock::now();
        glUseProgram(shader_program);
      
        // If modifying the vector field, display vector field. Otherwise, display the smoke.
        if (is_modify_vf) {
            for (int y = 0; y < NUMROW; ++y) {
                for (int x = 0; x < NUMCOL; ++x) {
                    Vector2D accumulated_direction = Vector2D(0.0, 0.0);
                    int count = 0;
                    for (int ys = -1 + y; ys <= y + 1; ys++) {
                        for (int xs = -1 + x; xs <= x + 1; xs++) {
                            if (ys >= 0 && xs >= 0 && ys < NUMROW && xs < NUMCOL) {
                                accumulated_direction += external_forces[ys*grid.width + xs];
                                ++count;
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
                        double angle = accumulated_direction.y >= 0 ? acos(accumulated_direction.x) : acos(accumulated_direction.x) + PI;
                        angle = angle * 180 / PI;
                        hue = angle;
                    }
                    
                    Vector3D rgb = hsv2rgb({hue, saturate, value});
//                    printf("len = %f: %f %f %f\n", len, rgb.x, rgb.y, rgb.z);

                    int index = y * NUMCOL + x;

                    int vertexColorLocation = glGetUniformLocation(shader_program, "ourColor");
                    glUniform4f(vertexColorLocation, rgb.x, rgb.y, rgb.z, 1.0f);
                    glBindVertexArray(VAOs[index]);
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                }
            }
        } else {
            for (int y = 0; y < NUMROW; ++y) {
                for (int x = 0; x < NUMCOL; ++x) {
                    double density = grid.getDensity(x, y);
                    double temperature = grid.getTemperature(x, y);

                    // [0, 100] -> [420, 320]
                    double hue = ((int)(450.0 - temperature * 1)) % 360;
                    // [0, 100]
                    double saturate = 100.0;
                    // [0, 100] -> [0, 100]
                    double value = density;

                    Vector3D rgb = hsv2rgb({hue, saturate, value});

                    int index = y * NUMCOL + x;

                    int vertexColorLocation = glGetUniformLocation(shader_program, "ourColor");
                    glUniform4f(vertexColorLocation, rgb.x, rgb.y, rgb.z, 1.0f);
                    glBindVertexArray(VAOs[index]);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//                    glDrawArrays(GL_TRIANGLES, 0, 6);
                }
            }
        }
        auto end_time = steady_clock::now();
        auto display_time = duration_cast<milliseconds>(end_time - start_time);
        if (rng() % 50 == 0) {
            printf("display_time = %lld mm\n", display_time.count());
        }
        screen->drawContents();
        screen->drawWidgets();

        glfwSwapBuffers(window);

    }
    glDeleteVertexArrays(NUMCOL * NUMROW, VAOs);
    glDeleteBuffers(NUMCOL * NUMROW, VBOs);
    glDeleteBuffers(NUMCOL * NUMROW, EBO);

    glfwTerminate();
    return 0;
}
