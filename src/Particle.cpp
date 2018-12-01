#include <math.h>
#include <GL/glut.h>

#include "Particle.h"

#include "shaders/simulation.frag.cpp"
#include "shaders/simulation.vert.cpp"

void Particle::init() {
}

void Particle::begin_render() {
}

void Particle::render() {
  if (!alive) {
    return;
  }

  glColor3f(1.0, 1.0, 1.0);
  glPushMatrix();
  // glVertex3f(pos.x, pos.y, pos.z);
  glTranslatef(pos.x, pos.y, pos.z);
  glutWireSphere(radius, 5, 5);
  glPopMatrix();
}

void Particle::end_render() {
}

// GLuint create_shader(GLenum type, const char* source) {
//   GLuint shader = 0;
//   GLint status;

//   shader = glCreateShader(type);

//   glShaderSource(shader, 1, (const GLchar**) &source, NULL);
//   glCompileShader(shader);
//   glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

//   if (status != GL_TRUE) {
//     std::cerr << "Failed to load shader." << std::endl;
//     glDeleteShader(shader);
//     exit(1);
//   }

//   return shader;
// }

// GLenum create_program() {
//   GLuint vtx_shader_id = create_shader(GL_VERTEX_SHADER, _file_simulation_vert);
//   GLuint frag_shader_id = create_shader(GL_VERTEX_SHADER, _file_simulation_frag);

//   GLint status;

//   program = glCreateProgram();

//   glAttachShader(program, vtx_shader_id);
//   glAttachShader(program, frag_shader_id);

//   // bind attrib location and fragment data location???? what is this?

//   glLinkProgram(program);
//   glGetProgramiv(program, GL_LINK_STATUS, &status);

//   // delete shader points, doesn't actually delete them internally (I hope)
//   glDeleteShader(vtx_shader_id);
//   glDeleteShader(frag_shader_id);

//   if (status != GL_TRUE) {
//     std::cerr << "Failed to link program." << std::endl;
//     glDeleteProgram(program);
//     exit(1);
//   }
// }
