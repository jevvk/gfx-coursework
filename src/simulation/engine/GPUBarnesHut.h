#pragma once

#include "Particle.h"

class GPUBarnesHut {
public:
  GPUBarnesHut(Particle* particles);
  ~GPUBarnesHut();

  void forward();

private:
  Particle* particles;

  void init_cl();
  void init_buffers();
  void init_cl_buffers();
  void init_gl_buffers();
  void load_kernels();
  void set_kernels_args();

  static float* _posX_buffer, _posY_buffer, _posY_buffer;
  static float* _velX_buffer, _velY_buffer, _velY_buffer;
  static float* _forceX_buffer, _forceY_buffer, _forceY_buffer;
  static float* _mass_buffer;
};
