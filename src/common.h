#ifndef common_h
#define common_h

#include "omp.h"

using std::mt19937;
using std::uniform_int_distribution;
using uni_dis = uniform_int_distribution<int>;
using namespace std::chrono;

const int NUMROW = 100;
const int NUMCOL = 100;
const int FREQ   = 20;
const int WINDOW_WIDTH = 640; // x is WIDTH
const int WINDOW_HEIGHT = 640; // y is HEIGHT

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "uniform vec4 ourColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = ourColor;\n"
                                   "}\n\0";

#endif