#ifndef callback_h
#define callback_h

#include <CGL/CGL.h>
#include "grid.h"
#include "common.h"

extern Grid grid;
extern Vector2D enter_cell;
extern Vector2D exit_cell;
//extern int size_smoke;
//extern double amount_smoke;
extern nanogui::Screen *screen;

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    screen->cursorPosCallbackEvent(xpos, ypos);
    glfwGetCursorPos(window, &grid.cursor_pos.x, &grid.cursor_pos.y);
}

// Handles mouse click to manually create smoke.
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    screen->mouseButtonCallbackEvent(button, action, mods);
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            Con::mouse_down = true;
            // Handle dragging of mouse to create a stream of smoke or modifying the vector field
        } else if (action == GLFW_RELEASE) {
            Con::mouse_down = false;
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
                Con::shift_pressed = true;
                break;
            case GLFW_KEY_RIGHT_SHIFT:
                Con::shift_pressed = true;
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
                Con::is_pause = !Con::is_pause;
                break;
            case GLFW_KEY_M:
                Con::is_modify_vf = !Con::is_modify_vf;
                break;
            case GLFW_KEY_R:
                Con::reset = true;
                break;
            default:
//                std::cout << key << " pressed" << std::endl;
                break;
        }
    } else if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_LEFT_SHIFT :
                Con::shift_pressed = false;
                break;
            case GLFW_KEY_RIGHT_SHIFT:
                Con::shift_pressed = false;
                break;
            default:
//                std::cout << key << " released" << std::endl;
                break;
        }
    }
}

#endif
