#include <math.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

#include "Particle.h"
#include "Constants.h"

#include "simulation/Camera.h"

unsigned int Particle::__gl_tex = 0;
unsigned int Particle::__gl_vert = 0;
unsigned int Particle::__gl_frag = 0;
unsigned int Particle::__gl_pos = 0;
unsigned int Particle::__gl_mv = 0;
unsigned int Particle::__gl_proj = 0;
unsigned int Particle::__gl_screensz = 0;
unsigned int Particle::__gl_spritesz = 0;

GLuint Particle::vao = 0;
GLuint Particle::vbo = 0;
GLuint Particle::eab = 0;

double* Particle::pos_buffer = NULL;
int Particle::n_count = 0;

glm::mat4 Particle::modelview = glm::mat4();
glm::mat4 Particle::projection = glm::mat4();

glm::vec2 Particle::screen = glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT);
float Particle::sprite_size = SPRITE_SIZE;

GLuint Particle::program = 0;

void checkCompileErrors(GLuint shader, char* name, int type);

void Particle::init() {
  // create vertex + fragment shader
  char* vert = (char*) _file_simulation_vert;
  char* frag = (char*) _file_simulation_frag;

  __gl_vert = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(__gl_vert, 1, (GLchar**) &vert, &_file_simulation_vert_length);
  glCompileShader(__gl_vert);
  checkCompileErrors(__gl_vert, "VERTEX", 0);

  __gl_frag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(__gl_frag, 1, (GLchar**) &frag, &_file_simulation_frag_length);
  glCompileShader(__gl_frag);
  checkCompileErrors(__gl_frag, "FRAGMENT", 0);

  // bind to program
  program = glCreateProgram();

  glAttachShader(program, __gl_vert);
  glAttachShader(program, __gl_frag);
  glLinkProgram(program);
  checkCompileErrors(program, "PROGRAM", 1);

  // delete shaders
  glDeleteShader(__gl_vert);
  glDeleteShader(__gl_frag);

  // create texture
  // glGenTextures(1, &__gl_tex);
  // glBindTexture(GL_TEXTURE_2D, __gl_tex); 

  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _file_body_bmp_width, _file_body_bmp_height, 0, GL_RGB, GL_UNSIGNED_BYTE, _file_body_bmp);
  // glGenerateMipmap(GL_TEXTURE_2D);

  // assign texture
  // glUseProgram(program);
  // glUniform1i(glGetUniformLocation(program, "tex"), 0);

  projection = glm::perspective(glm::radians(60.0f), (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT, 10.0f, 80000000.0f);
  __gl_proj = glGetUniformLocation(program, "projection");

  glUniformMatrix4fv(__gl_proj, 1, GL_FALSE, glm::value_ptr(projection));

  __gl_screensz = glGetUniformLocation(program, "screenSize");
  __gl_spritesz = glGetUniformLocation(program, "spriteSize");

  glUniform2f(__gl_screensz, screen.x, screen.y);
  glUniform1f(__gl_spritesz, sprite_size);

  glPointSize(3);
}

void Particle::init_buffer(Particle* particles, int count) {
  n_count = count;
  pos_buffer = new double[n_count * 3];

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);

  __gl_pos = glGetAttribLocation(program, "position");

  update_buffer(particles);

  glVertexAttribPointer(__gl_pos, 3, GL_DOUBLE, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(__gl_pos);
}

void Particle::update_buffer(Particle* particles) {
  for (int i = 0; i < n_count; ++i) {
    Particle P = particles[i];

    pos_buffer[i * 3] = P.pos.x;
    pos_buffer[i * 3 + 1] = P.pos.y;
    pos_buffer[i * 3 + 2] = P.pos.z;
  }

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buffer), pos_buffer, GL_DYNAMIC_DRAW);
}

void Particle::render_all(Camera cam) {
  // glActiveTexture(GL_TEXTURE0);
  // glBindTexture(GL_TEXTURE_2D, __gl_tex);

  modelview = glm::lookAt(glm::vec3(cam.eyex, cam.eyey, cam.eyez),
                          glm::vec3(cam.centerx, cam.centery, cam.centerz),
                          glm::vec3(cam.upx, cam.upz, cam.upy));

  __gl_mv = glGetUniformLocation(program, "modelview");

  glUniformMatrix4fv(__gl_mv, 1, GL_FALSE, glm::value_ptr(modelview));

  glUseProgram(program);
  glBindVertexArray(vao);
  glDrawArrays(GL_POINTS, 0, n_count);
}

void Particle::exit() {
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
}

// void Particle::render() {
//   if (!alive) {
//     return;
//   }

//   glColor3f(1.0, 1.0, 1.0);
//   glPushMatrix();
//   // glVertex3f(pos.x, pos.y, pos.z);
//   glTranslatef(pos.x, pos.y, pos.z);
//   glutWireSphere(radius, 5, 5);
//   glPopMatrix();
// }

void checkCompileErrors(GLuint shader, char* name, int type) {
  GLint success;
  GLchar infoLog[1024];

  if (type == 1) {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
  } else {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  }

  if (!success) {
    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
    std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << name << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    exit(1);
  }
}