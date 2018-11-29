#pragma once

#include "universe/Universe.h"
#include "simulation/Camera.h"

#define N_PARTICLES 300
#define N_SAMPLES 20
#define GRAVITY_CONST 6.67e-11
#define TIME_STEP 7e-2
#define EPSILON 1e-11

class Simulation {
public:
  Simulation(Universe* universe);

  void reset();
  void handle_key(unsigned char key);
  void handle_specialkey(int key);

  virtual void _reset() {};
  virtual void render() {};
  virtual void step() {};

protected:
  Universe* universe;
  Camera* camera;
  Particle* particles;

  bool paused = true;
  unsigned char last_key = 0;

  float gravity_const = GRAVITY_CONST;
  float n_samples = N_SAMPLES;
  float time_step = TIME_STEP;

  virtual void _handle_key(unsigned char key) {};
  virtual void _handle_specialkey(int key) {};

  void debug_camera();
  void debug_particles();
  
  static void draw_axes();
  static void debug_particle(Particle P, const char* name);
};
