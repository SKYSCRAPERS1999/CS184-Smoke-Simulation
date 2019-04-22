#include <iostream>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <random>
#include "grid.h"
#include <time.h>

#define NUMROW 50
#define NUMCOL 50
//float grid[NUMROW][NUMCOL][3];

// starts a smoke at a random location
void randomize_grid(Grid &grid) {
  int chosenx = rand() % NUMCOL;
  int choseny = rand() % NUMROW;

  grid.setDensity(chosenx, choseny, 75);
}

void display(const Grid& grid) {
  glClear(GL_COLOR_BUFFER_BIT);
  float width = 1 / (float) NUMCOL * 2;
  float height = 1 / (float) NUMROW * 2;

  for (int i = 0; i < NUMROW; ++i) {
    for (int j = 0; j < NUMCOL; ++j) {
      glColor3f(grid.getDensity(j, i) / 100, grid.getDensity(j, i) / 100, grid.getDensity(j, i) / 100);
      //glColor3f((double)j/NUMCOL, (double)j/NUMCOL, (double)j/NUMCOL);

      glBegin(GL_QUADS);
      float bottom_left_x = -1 + width * j;
      float bottom_left_y = -1 + height * i;

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
  GLFWwindow *window;

  Grid grid = Grid(NUMCOL, NUMROW);
  // Initialize
  if (!glfwInit()) {
    return -1;
  }
  window = glfwCreateWindow(640, 480, "Smoke Simulation", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  // Randomize grid
  srand((unsigned) time(NULL));

  time_t last_time;
  time(&last_time);


  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // To prevent screen tearing
  while (!glfwWindowShouldClose(window)) {
    time_t cur_time;
    time(&cur_time);

    if (difftime(cur_time, last_time) >= 0.5) {
      //std::cout << "Update the grid" << std::endl;
      last_time = cur_time;
      grid.simulate(1);
      randomize_grid(grid);
      //grid.printGrid();
    }
    display(grid);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
