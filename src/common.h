#ifndef common_h
#define common_h

#include "omp.h"

#include <random>
#include <nanogui/nanogui.h>

using std::mt19937;
using std::uniform_int_distribution;
using uni_dis = uniform_int_distribution<int>;
using namespace std::chrono;

// constats

class Con {

public:

  static constexpr int NUMCOL = 500;
  static constexpr int NUMROW = 500;
  static constexpr int FREQ = 30;
  static int WINDOW_WIDTH; // x is WIDTH
  static int WINDOW_HEIGHT; // y is HEIGHT
  static constexpr int DISPLAY_LIMIT = 1;
  static constexpr double EPS = 1e-3;

  static const GLchar *vertexShaderSource;
  static const GLchar *fragmentShaderSource;

};


#endif
