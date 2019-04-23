#include <iostream>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <random>
#include "grid.h"
#include <time.h>
#include <chrono>
using std::mt19937;
using std::uniform_int_distribution;
using uni_dis = uniform_int_distribution<int>;

const int NUMROW = 50;
const int NUMCOL = 50;
const int FREQ   = 30;

static std::random_device rd;
static mt19937 rng(rd()); // random number generator in C++11

using namespace std::chrono;

// starts a smoke at a random location
void randomize_grid(Grid &grid, int iter = 3) {
  uni_dis dis_x(0, NUMCOL - 1); // uniform distribution in C++11
  uni_dis dis_y(0, NUMROW - 1); // uniform distribution in C++11
  uni_dis dis_density(25, 75); // uniform distribution in C++11
  while (iter--) {
    int chosenx = dis_x(rng);
    int choseny = dis_y(rng);
    grid.setDensity(chosenx, choseny, dis_density(rng));
  }
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


  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // To prevent screen tearing

  auto last_time = steady_clock::now();
  while (!glfwWindowShouldClose(window)) {
    auto cur_time = steady_clock::now();
    auto elapsed = duration_cast<milliseconds>(cur_time - last_time);

    if (FREQ * elapsed.count() >= 1000) {
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
