#pragma once

#include <CL/cl.hpp>

#include "Particle.h"

class GPUBarnesHut {
public:
  GPUBarnesHut(Particle* particles, int count);
  ~GPUBarnesHut();

  void forward();

private:
  Particle* particles;
  int n_particles;

  cl::Context context;
  cl::Program program;

  void init_cl();
  void init_buffers();
  void init_cl_buffers();
  void init_gl_buffers();
  void load_kernels();
  void set_kernels_args();

  static float* _posX_buffer;
  static float* _posY_buffer;
  static float* _posZ_buffer;

  static float* _velX_buffer;
  static float* _velY_buffer;
  static float* _velZ_buffer;

  static float* _forceX_buffer;
  static float* _forceY_buffer;
  static float* _forceZ_buffer;

  static float* _mass_buffer;
};
