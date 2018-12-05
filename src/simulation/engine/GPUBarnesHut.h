#pragma once

#include <vector>
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
  int n_nodes;

  cl::Context context;
  cl::Device device;
  cl::Program program;
  std::vector<cl::Kernel> kernels;

  float* _posX_buffer;
  float* _posY_buffer;
  float* _posZ_buffer;

  float* _velX_buffer;
  float* _velY_buffer;
  float* _velZ_buffer;

  float* _forceX_buffer;
  float* _forceY_buffer;
  float* _forceZ_buffer;

  float* _mass_buffer;

  void init_cl();
  void init_buffers();
  void init_cl_buffers();
  void init_gl_buffers();
  void load_kernels();
  void set_kernels_args();
  std::string get_options();
};
