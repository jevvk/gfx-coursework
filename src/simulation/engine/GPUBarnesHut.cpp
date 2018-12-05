#include <CL/cl.hpp>
#include <iostream>
#include <vector>

#include "Config.h"
#include "Files.h"

#include "simulation/engine/GPUBarnesHut.h"

GPUBarnesHut::GPUBarnesHut(Particle* particles) : particles(particles) {
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

  cl::Context context({ default_device });
  cl::Program::Sources sources;
}
