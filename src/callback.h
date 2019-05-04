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

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    glfwGetCursorPos(window, &grid.cursor_pos[0], &grid.cursor_pos[1]);
}

// Handles mouse click to manually create smoke.
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mouse_down = true;

            double xpos = grid.cursor_pos[0];
            double ypos = grid.cursor_pos[1];

            int row = int(NUMROW - NUMROW * ypos / double(WINDOW_HEIGHT));
            int col = int(NUMCOL * xpos / double(WINDOW_WIDTH));

            double den = grid.getDensity(col, row);
            grid.setDensity(col, row, std::max(den + 25, 100.0));
        } else if (action == GLFW_RELEASE) {
            mouse_down = false;
        }
    }
}

void keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
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
                amount_smoke = max(1.0, amount_smoke - 1.0);
                std::cout << "Amount of smoke is " << amount_smoke << std::endl;
                break;
            case ']':
                amount_smoke = min(100.0, amount_smoke + 1.0);
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
