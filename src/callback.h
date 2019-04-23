#ifndef callback_h
#define callback_h

extern Grid grid;

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
  glfwGetCursorPos(window, &grid.cursor_pos[0], &grid.cursor_pos[1]);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  if (action == GLFW_PRESS) {
    double xpos = grid.cursor_pos[0];
    double ypos = grid.cursor_pos[1];

    int row = int(NUMROW - NUMROW * ypos / double(WINDOW_HEIGHT));
    int col = int(NUMCOL * xpos / double(WINDOW_WIDTH));

    double den = grid.getDensity(col, row);
    grid.setDensity(col, row, std::max(den + 25, 100.0));
  }
}

#endif
