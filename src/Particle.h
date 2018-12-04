#pragma once

#include <GL/gl.h>
#include <glm/glm.hpp>

#include "Vec3.h"
#include "Files.h"

#include "simulation/Camera.h"

class Particle {
public:
  Vec3 pos, vel, force, _force;
  float mass;
  float radius;
  bool alive;

  static void init();
  static void exit();
  static void init_buffer(Particle* particles, int count);
  static void update_buffer(Particle* particles);
  static void render_all(Camera camera);

private:
  static GLuint create_shader(GLenum type, const char* source);
  static GLenum create_program();

  static GLuint program;
  static unsigned int __gl_vert, __gl_frag;
  static unsigned int __gl_tex;
  static unsigned int __gl_pos;
  static unsigned int __gl_mv, __gl_proj;
  static unsigned int __gl_screensz, __gl_spritesz;

  static GLuint vbo, vao, eab;

  static float* pos_buffer;
  static int n_count;

  static glm::mat4 modelview, projection;
  static glm::vec2 screen;
  static float sprite_size;
};
