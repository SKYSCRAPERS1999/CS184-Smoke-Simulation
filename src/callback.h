#ifndef callback_h
#define callback_h

extern Grid grid;
// global mouse_down variable used in main.cpp
extern bool mouse_down;
// global is_pause variable used in main.cpp
extern bool is_pause;
// global variables to control size of smoke
extern bool shift_pressed;
extern int size_smoke;
extern double amount_smoke;
extern nanogui::Screen *screen;

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
//    screen->cursorPosCallbackEvent(xpos, ypos);
    glfwGetCursorPos(window, &grid.cursor_pos.x, &grid.cursor_pos.y);
}

void error_callback(int error, const char* description) {
    puts(description);
}

// Handles mouse click to manually create smoke.
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
//    screen->mouseButtonCallbackEvent(button, action, mods);
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mouse_down = true;
            std::cout<<"Mouse Down!"<<std::endl;
        } else if (action == GLFW_RELEASE) {
            mouse_down = false;
        }
    }
}

void keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
//    screen->keyCallbackEvent(key, scancode, action, mods);
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_LEFT_SHIFT :
                shift_pressed = true;
                break;
            case GLFW_KEY_RIGHT_SHIFT:
                shift_pressed = true;
                break;
            case '=':
                if (shift_pressed) size_smoke = min(min(grid.width, grid.height), size_smoke + 1);
                std::cout << "Size of smoke is " << size_smoke << std::endl;
                break;
            case '-':
                size_smoke = max(1, size_smoke - 1);
                std::cout << "Size of smoke is " << size_smoke << std::endl;
                break;
            case '[':
                amount_smoke = max(1.0, amount_smoke - 2.0);
                std::cout << "Amount of smoke is " << amount_smoke << std::endl;
                break;
            case ']':
                amount_smoke = min(100.0, amount_smoke + 2.0);
                std::cout << "Amount of smoke is " << amount_smoke << std::endl;
                break;
            case GLFW_KEY_P:
                is_pause = !is_pause;
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

#endif
