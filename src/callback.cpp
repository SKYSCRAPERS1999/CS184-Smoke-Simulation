#ifndef callback_h
#define callback_h

#include "grid.h"
#include <CGL/CGL.h>
#include <nanogui/nanogui.h>
#include <random>
#include "common.h"

extern Grid grid;
// global variables to control simulation in main.cpp
extern bool mouse_down;
extern bool is_pause;
extern bool shift_pressed;
extern bool is_modify_vf;
extern bool reset;
extern Vector2D enter_cell;
extern Vector2D exit_cell;
extern int size_smoke;
extern double amount_smoke;
extern nanogui::Screen *screen;

extern int size_mouse;
extern double amount_temperature;
extern vector<Vector2D> external_forces;
extern mt19937 rng;

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    screen->cursorPosCallbackEvent(xpos, ypos);
    glfwGetCursorPos(window, &grid.cursor_pos.x, &grid.cursor_pos.y);
}

void error_callback(int error, const char *description) {
    puts(description);
}

// Handles mouse click to manually create smoke.
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    screen->mouseButtonCallbackEvent(button, action, mods);
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mouse_down = true;
            // Handle dragging of mouse to create a stream of smoke or modifying the vector field
        } else if (action == GLFW_RELEASE) {
            mouse_down = false;
        }
    }

}

void window_size_callback(GLFWwindow *main_window, int width, int height) {
    Con::WINDOW_WIDTH = width;
    Con::WINDOW_HEIGHT = height;
}

void keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    screen->keyCallbackEvent(key, scancode, action, mods);
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_LEFT_SHIFT :
                shift_pressed = true;
                break;
            case GLFW_KEY_RIGHT_SHIFT:
                shift_pressed = true;
                break;
//            case '=':
//                if (shift_pressed) size_smoke = min(min(grid.width, grid.height), size_smoke + 1);
//                std::cout << "Size of smoke is " << size_smoke << std::endl;
//                break;
//            case '-':
//                size_smoke = max(1, size_smoke - 1);
//                std::cout << "Size of smoke is " << size_smoke << std::endl;
//                break;
//            case '[':
//                amount_smoke = max(1.0, amount_smoke - 2.0);
//                std::cout << "Amount of smoke is " << amount_smoke << std::endl;
//                break;
//            case ']':
//                amount_smoke = min(100.0, amount_smoke + 2.0);
//                std::cout << "Amount of smoke is " << amount_smoke << std::endl;
//                break;
            case GLFW_KEY_P:
                is_pause = !is_pause;
                break;
            case GLFW_KEY_M:
                is_modify_vf = !is_modify_vf;
                break;
            case GLFW_KEY_R:
                reset = true;
                break;
            default:
//                std::cout << key << " pressed" << std::endl;
                break;
        }
    } else if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_LEFT_SHIFT :
                shift_pressed = false;
                break;
            case GLFW_KEY_RIGHT_SHIFT:
                shift_pressed = false;
                break;
            default:
//                std::cout << key << " released" << std::endl;
                break;
        }
    }
}

void set_callback(GLFWwindow *window) {
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

#endif
