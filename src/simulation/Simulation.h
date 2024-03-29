#pragma once

#include "universe/Universe.h"
#include "simulation/Camera.h"

#define GRAVITY_CONST 6.67e-11
#define TIME_STEP 7e-3
#define EPSILON 0.15

class Simulation {
public:
  Simulation(Universe* universe, int particle_count);

  void reset();
  void render();
  void handle_key(unsigned char key);
  void handle_specialkey(int key);

  virtual void _reset() {};
  virtual void step() {};

protected:
  Universe* universe;
  Camera* camera;
  Particle* particles;

  bool paused = true;
  unsigned char last_key = 0;

  int n_particles = 0;
  float gravity_const = GRAVITY_CONST;
  float time_step = TIME_STEP;

  virtual void _handle_key(unsigned char key) {};
  virtual void _handle_specialkey(int key) {};

  void debug_camera();
  void debug_particles();
  
  static void draw_axes();
  static void debug_particle(Particle P, const char* name);
};
