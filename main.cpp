#include <iostream>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <random>
#include "grid.h"
#include <time.h>

#define NUMROW 15
#define NUMCOL 15

std::mt19937 rng(0); // random number generator in C++11

// starts a smoke at a random location
void randomize_grid(Grid &grid) {
  int chosenx = rng() % NUMCOL;
  int choseny = rng() % NUMROW;

  grid.setDensity(chosenx, choseny, 75);
}

void display(const Grid& grid) {
  glClear(GL_COLOR_BUFFER_BIT);
  double width = 1 / (double) NUMCOL * 2;
  double height = 1 / (double) NUMROW * 2;

  for (int y = 0; y < NUMROW; ++y) {
    for (int x = 0; x < NUMCOL; ++x) {
      glColor3d(grid.getDensity(x, y) / 100, grid.getDensity(x, y) / 100, grid.getDensity(x, y) / 100);
      //glColor3d((double)x/NUMCOL, (double)x/NUMCOL, (double)x/NUMCOL);

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
