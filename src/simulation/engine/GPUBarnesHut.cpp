#include <CL/cl.hpp>
#include <iostream>
#include <vector>

#include "Config.h"
#include "Files.h"
#include "Particle.h"

#include "simulation/engine/GPUBarnesHut.h"

float* GPUBarnesHut::_posX_buffer = NULL;
float* GPUBarnesHut::_posY_buffer = NULL;
float* GPUBarnesHut::_posZ_buffer = NULL;

float* GPUBarnesHut::_velX_buffer = NULL;
float* GPUBarnesHut::_velY_buffer = NULL;
float* GPUBarnesHut::_velZ_buffer = NULL;

float* GPUBarnesHut::_forceX_buffer = NULL;
float* GPUBarnesHut::_forceY_buffer = NULL;
float* GPUBarnesHut::_forceZ_buffer = NULL;

float* GPUBarnesHut::_mass_buffer = NULL;

GPUBarnesHut::GPUBarnesHut(Particle* particles, int count) : particles(particles), n_particles(count) {
  init_cl();
  init_buffers();
  init_gl_buffers();
  init_cl_buffers();
  load_kernels();
  set_kernels_args();
}

void GPUBarnesHut::init_cl() {
  std::vector<cl::Platform> all_platforms;
  cl::Platform::get(&all_platforms);
  
  if (all_platforms.size() == 0) {
    std::cout << "GPUBarnesHut: No platforms found. Check OpenCL installation." << std::endl;
    exit(1);
  }

  cl::Platform default_platform = all_platforms[opencl_device_choice];
  std::cout << "GPUBarnesHut: Using platform " << default_platform.getInfo<CL_PLATFORM_NAME>() << std::endl;

  std::vector<cl::Device> all_devices;
  default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);

  if (all_devices.size() == 0) {
    std::cout << "GPUBarnesHut: No devices found. Check OpenCL installation." << std::endl;
    exit(1);
  }

  cl::Device default_device = all_devices[0];
  std::cout << "GPUBarnesHut: Using device " << default_device.getInfo<CL_DEVICE_NAME>() << std::endl;

  context = cl::Context({ default_device });
}

// TODO: kernels have more buffers
void GPUBarnesHut::init_buffers() {
  _posX_buffer = new float[n_particles];
  _posY_buffer = new float[n_particles];
  _posZ_buffer = new float[n_particles];

  _velX_buffer = new float[n_particles];
  _velY_buffer = new float[n_particles];
  _velZ_buffer = new float[n_particles];

  _forceX_buffer = new float[n_particles];
  _forceY_buffer = new float[n_particles];
  _forceZ_buffer = new float[n_particles];

  _mass_buffer = new float[n_particles];

  for (int i = 0; i < n_particles; ++i) {
    Particle P = particles[i];

    _posX_buffer[i] = P.pos.x;
    _posY_buffer[i] = P.pos.y;
    _posZ_buffer[i] = P.pos.z;

    _velX_buffer[i] = P.vel.x;
    _velY_buffer[i] = P.vel.y;
    _velZ_buffer[i] = P.vel.z;

    _forceX_buffer[i] = 0;
    _forceY_buffer[i] = 0;
    _forceZ_buffer[i] = 0;
  }
}

// TODO
void GPUBarnesHut::init_cl_buffers() {
  
}


// TODO
void GPUBarnesHut::init_gl_buffers() {
  
}

void GPUBarnesHut::load_kernels() {
  // there has to be a better way
  std::pair<const char*, long unsigned int> boundingbox = std::pair(_file_boundingbox_cl, _file_boundingbox_cl_length);
  std::pair<const char*, long unsigned int> buildtree = std::pair(_file_buildtree_cl, _file_buildtree_cl_length);
  std::pair<const char*, long unsigned int> calculateforce = std::pair(_file_calculateforce_cl, _file_calculateforce_cl_length);
  std::pair<const char*, long unsigned int> copyvertices = std::pair(_file_copyvertices_cl, _file_copyvertices_cl_length);
  std::pair<const char*, long unsigned int> integrate = std::pair(_file_integrate_cl, _file_integrate_cl_length);
  std::pair<const char*, long unsigned int> sort = std::pair(_file_sort_cl, _file_sort_cl_length);
  std::pair<const char*, long unsigned int> summarizetree = std::pair(_file_summarizetree_cl, _file_summarizetree_cl_length);

  std::vector<std::pair<const char*, long unsigned int>> kernels = { boundingbox, buildtree, calculateforce, copyvertices,
                                       integrate, sort, summarizetree };

  program = cl::Program(context, kernels);
}

// TODO
void GPUBarnesHut::set_kernels_args() {
  
}
