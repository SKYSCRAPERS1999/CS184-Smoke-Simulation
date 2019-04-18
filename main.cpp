#include <iostream>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <random>


#define NUMROW 10
#define NUMCOL 10
float grid[NUMROW][NUMCOL][3];

void randomize_grid() {
    for (int i = 0; i < NUMROW; ++i) {
        for (int j = 0; j < NUMCOL; ++j) {
            for (int k = 0; k < 3; ++k) {
                grid[i][j][k] = (rand() % 100) / (float)100;
            }
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    float width = 1 / (float)NUMCOL * 2;
    float height = 1 / (float)NUMROW * 2;

    for (int i = 0; i < NUMROW; ++i) {
        for (int j = 0; j < NUMCOL; ++j) {
            glColor3f(grid[i][j][0], grid[i][j][1], grid[i][j][2]);
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
    randomize_grid();

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // To prevent screen tearing
    unsigned prev_time = time(NULL);
    while (!glfwWindowShouldClose(window)) {
        if (time(NULL) != prev_time) {
            randomize_grid();
            display();
            prev_time - time(NULL);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}