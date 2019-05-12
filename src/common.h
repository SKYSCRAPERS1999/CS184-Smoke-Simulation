#ifndef common_h
#define common_h

//#include "omp.h"
#include "grid.h"

#include <random>
#include <nanogui/nanogui.h>
#include "CGL/vector2D.h"
#include "CGL/vector3D.h"
#include "CGL/vector4D.h"

using std::mt19937;
using std::uniform_int_distribution;
using uni_dis = uniform_int_distribution<int>;
using namespace std::chrono;

// constants

class Con {

public:

  // basic constants

  static constexpr int NUMCOL = 500;
  static constexpr int NUMROW = 300;

  static constexpr int FREQ = 30;
  static constexpr int DISPLAY_LIMIT = 1;
  static constexpr double EPS = 1e-3;

  static int WINDOW_WIDTH; // x is WIDTH
  static int WINDOW_HEIGHT; // y is HEIGHT

  // for callbacks

  static bool mouse_down;
  static bool is_pause;
  static bool shift_pressed;
  static bool is_modify_vf;
  static bool reset;
  static bool debug;
  static int size_mouse;
  static double ALPHA;

  // Adjustable parameters for nanogui

  static int size_smoke;
  static double amount_smoke;
  static double amount_temperature;
  static double ambient_temperature;
  static double temperature_parameter;
  static double smoke_density_parameter;
  static double external_force_parameter;
  static double num_iter;

  static Vector3D picked_rgb;

  // shader sources

  static const GLchar *vertexShaderSource;
  static const GLchar *fragmentShaderSource;

  static Vector2D enter_cell;
  static Vector2D exit_cell;

  static std::random_device rd;

  // random number generator in C++11
  static mt19937 rng;

  // Vertex Array Object and Vertex Buffer Object
  static GLuint VAO, VBO, EBO;
  static GLuint texture;
  static GLuint shader_program;
};


#endif
