#ifndef common_h
#define common_h

#include "omp.h"

using std::mt19937;
using std::uniform_int_distribution;
using uni_dis = uniform_int_distribution<int>;
using namespace std::chrono;

const int NUMCOL = 500;
const int NUMROW = 500;
const int FREQ = 30;

int WINDOW_WIDTH = 700; // x is WIDTH
int WINDOW_HEIGHT = 640; // y is HEIGHT
const int DISPLAY_LIMIT = 1;
const double EPS = 1e-3;

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

const GLchar *vertexShaderSource = R"glsl(
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

const GLchar *fragmentShaderSource = R"glsl(
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

#endif
