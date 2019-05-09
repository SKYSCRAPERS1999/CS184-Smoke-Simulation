//
// Created by impulse on 2019-05-09.
//
#include "common.h"

int Con::WINDOW_WIDTH = 700;
int Con::WINDOW_HEIGHT = 640;

bool Con::mouse_down = false;
bool Con::is_pause = false;
bool Con::shift_pressed = false;
bool Con::is_modify_vf = false;
bool Con::reset = false;
bool Con::debug = true;

const GLchar* Con::vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    layout (location = 2) in vec2 aTexCoord;

    out vec3 ourColor;
    out vec2 TexCoord;

    void main()
    {
        gl_Position = vec4(aPos, 1.0);
        ourColor = aColor;
        TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    }
  )glsl";

const GLchar* Con::fragmentShaderSource = R"glsl(
  #version 330 core
  out vec4 FragColor;

  in vec3 ourColor;
  in vec2 TexCoord;

  // texture sampler
  uniform sampler2D texture1;

  void main()
  {
      FragColor = texture(texture1, TexCoord);
  }
)glsl";


//const char *vertexShaderSource = "#version 330 core\n"
//                                 "layout (location = 0) in vec3 aPos;\n"
//                                 "void main()\n"
//                                 "{\n"
//                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
//                                 "}\0";
//const char *fragmentShaderSource = "#version 330 core\n"
//                                   "out vec4 FragColor;\n"
//                                   "uniform vec4 ourColor;\n"
//                                   "void main()\n"
//                                   "{\n"
//                                   "   FragColor = ourColor;\n"
//                                   "}\n\0";