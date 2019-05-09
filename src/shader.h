//
// Created by impulse on 2019-05-09.
//
#include "common.h"


void generate_vertices_array() {
  glGenVertexArrays(1, &Con::VAO);
  glGenBuffers(1, &Con::VBO);
  glGenBuffers(1, &Con::EBO);
  glGenTextures(1, &Con::texture);
  GLuint elements[] = {
      0, 1, 2,
      2, 3, 0
  };
  float vertices[] = {
      // positions          // colors           // tecture coords
      1, 1, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
      1, -1, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
      -1, -1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
      -1, 1, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
  };
  glBindVertexArray(Con::VAO);
  glBindBuffer(GL_ARRAY_BUFFER, Con::VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Con::EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
  // position attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) 0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) (2 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // Con::tecture coord attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) (5 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindTexture(GL_TEXTURE_2D, Con::texture);
  // set Con::tecture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set Con::tecture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void build_shader_program() {
  // vertex shader
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &Con::vertexShaderSource, NULL);
  glCompileShader(vertex_shader);
  // check for compile errors
  int success;
  char info_log[512];
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
  }
  // fragment shader
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &Con::fragmentShaderSource, NULL);
  glCompileShader(fragment_shader);
  // check for compile error
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
  }
  // link shaders
  Con::shader_program = glCreateProgram();
  glAttachShader(Con::shader_program, vertex_shader);
  glAttachShader(Con::shader_program, fragment_shader);
  glBindFragDataLocation(Con::shader_program, 0, "outColor"); // TODO not sure
  glLinkProgram(Con::shader_program);
  // check for link error
  glGetProgramiv(Con::shader_program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(Con::shader_program, 512, NULL, info_log);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
  }
  // delete shader
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}
