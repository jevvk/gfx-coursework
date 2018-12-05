#include <CL/cl.hpp>
#include <iostream>
#include <vector>

#include "Config.h"
#include "Files.h"
#include "Particle.h"

#include "simulation/engine/GPUBarnesHut.h"

#define MAX_COMPUTE_UNITS 16
#define WARPSIZE 16
#define WORKGROUP_SIZE 16
#define NUM_WORK_GROUPS 16

// TODO: wtf is this
int wtf_is_this_doing(int x, int y) {
  int numberOfNodes = x * 2;

  if (numberOfNodes < 1024 * y)
    numberOfNodes = 1024 * y;
  while ((numberOfNodes & (WARPSIZE - 1)) != 0)
    ++numberOfNodes;

  return numberOfNodes;
}

GPUBarnesHut::GPUBarnesHut(Particle* particles, int count) : particles(particles), n_particles(count), n_nodes(wtf_is_this_doing(count, MAX_COMPUTE_UNITS)) {
  init_cl();
  init_buffers();
  init_gl_buffers();
  init_cl_buffers();
  load_kernels();
  set_kernels_args();
}

GPUBarnesHut::~GPUBarnesHut() {
  free(_posX_buffer);
  free(_posY_buffer);
  free(_posZ_buffer);

  free(_velX_buffer);
  free(_velY_buffer);
  free(_velZ_buffer);

  free(_forceX_buffer);
  free(_forceY_buffer);
  free(_forceZ_buffer);

  free(_mass_buffer);
}

void GPUBarnesHut::init_cl() {
  std::vector<cl::Platform> all_platforms;
  cl::Platform::get(&all_platforms);
  
  if (all_platforms.size() == 0) {
    std::cerr << "GPUBarnesHut: No platforms found. Check OpenCL installation." << std::endl;
    exit(1);
  }

  cl::Platform default_platform = all_platforms[opencl_device_choice];
  std::cout << "GPUBarnesHut: Using platform " << default_platform.getInfo<CL_PLATFORM_NAME>() << std::endl;

  std::vector<cl::Device> all_devices;
  default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);

  if (all_devices.size() == 0) {
    std::cerr << "GPUBarnesHut: No devices found. Check OpenCL installation." << std::endl;
    exit(1);
  }

  device = all_devices[0];
  std::cout << "GPUBarnesHut: Using device " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
  std::cout << "GPUBarnesHut: Using device " << device.getInfo<CL_DEVICE_VERSION>() << std::endl;

  context = cl::Context({ device });
}

void GPUBarnesHut::init_buffers() {
  _posX_buffer = new float[n_nodes + 1];
  _posY_buffer = new float[n_nodes + 1];
  _posZ_buffer = new float[n_nodes + 1];

  _velX_buffer = new float[n_nodes + 1];
  _velY_buffer = new float[n_nodes + 1];
  _velZ_buffer = new float[n_nodes + 1];

  _forceX_buffer = new float[n_nodes + 1];
  _forceY_buffer = new float[n_nodes + 1];
  _forceZ_buffer = new float[n_nodes + 1];

  _mass_buffer = new float[n_nodes + 1];

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

  // TODO more buffers
}

// TODO
void GPUBarnesHut::init_cl_buffers() {
  
}

// TODO
void GPUBarnesHut::init_gl_buffers() {
  
}

void GPUBarnesHut::load_kernels() {
  cl::Program::Sources sources;

  sources.push_back({ _file_boundingbox_cl, _file_boundingbox_cl_length });
  sources.push_back({ _file_buildtree_cl, _file_buildtree_cl_length });
  sources.push_back({ _file_calculateforce_cl, _file_calculateforce_cl_length });
  sources.push_back({ _file_copyvertices_cl, _file_copyvertices_cl_length });
  sources.push_back({ _file_integrate_cl, _file_integrate_cl_length });
  sources.push_back({ _file_sort_cl, _file_sort_cl_length });
  sources.push_back({ _file_summarizetree_cl, _file_summarizetree_cl_length });

  program = cl::Program(context, sources);

  if (program.build({ device }, get_options().c_str()) != CL_SUCCESS) {
    cl_int buildErr = CL_SUCCESS;
    auto error = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device, &buildErr);

    std::cerr << "GPUBarnesHut: Couldn't compile OpenCL program." << std::endl;
    std::cerr << error << std::endl;
    exit(1);
  }

  if (program.createKernels(&kernels) != CL_SUCCESS) {
    std::cerr << "GPUBarnesHut: Couldn't create OpenCL kernels." << std::endl;
    exit(1);
  }
}

// TODO
void GPUBarnesHut::set_kernels_args() {
  
}

std::string GPUBarnesHut::get_options() {
  std::string options = "-cl-std=CL2.0";
  
  options += " -D NUMBER_OF_NODES=";
  options += std::to_string(n_nodes);

  options += " -D NBODIES=";
  options += std::to_string(n_particles);

  options += " -D WORKGROUP_SIZE=";
  options += std::to_string(WORKGROUP_SIZE);

  options += " -D NUM_WORK_GROUPS=";
  options += std::to_string(NUM_WORK_GROUPS);
}
