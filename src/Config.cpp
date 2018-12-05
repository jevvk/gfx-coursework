#include <algorithm>
#include <cstring>
#include <iostream>

#include "Config.h"

UniverseChoice universe_choice = RANDOM_SPHERE;
SimulationChoice simulation_choice = CPU_NAIVE;
int opencl_device_choice = 0;
int n_bodies = 0;

char** arg_start;
char** arg_end;

void print_usage() {
	std::cout << "Usage: gfx-coursework [OPTION]...\n";
  std::cout << "Options: " << std::endl;
  std::cout << "\t" << "--bodies" << "\t" << "number of bodies" << std::endl;
  std::cout << "\t" << "--universe" << "\t" << "twin (default), random" << std::endl;
  std::cout << "\t" << "--simulation" << "\t" << "cpu-naive, gpu-naive, cpu-barnes-hut, gpu-barnes-hut (default)" << std::endl;
  std::cout << "\t" << "--render" << "\t" << "sphere(default), shader" << std::endl;
  std::cout << "\t" << "--opencl-device" << "\t" << "0, 1" << std::endl;
}

char* cmd_option_value(const std::string & option) {
  char ** itr = std::find(arg_start, arg_end, option);

  return (itr != arg_end && ++itr != arg_end && *itr[0] != '-') ? *itr : 0;
}

bool cmd_option_exists(const std::string& option) {
  return std::find(arg_start, arg_end, option) != arg_end;
}

void get_options(int argc, char * argv[]) {
  arg_start = argv;
  arg_end = argv + argc;

  if (cmd_option_exists("-h")) {
    print_usage();
    exit(0);
  }

  {
    char* u_selection;

    if (u_selection = cmd_option_value("--universe")) {
      if (std::strcmp(u_selection, "twin") == 0) {
        universe_choice = TWIN_COLLISION;
      } else if (std::strcmp(u_selection, "random") == 0) {
        universe_choice = RANDOM_SPHERE;
      } else {
        print_usage();
        exit(0);
      }
    }
  }

  {
    char* s_selection;

    if (s_selection = cmd_option_value("--simulation")) {
      if (std::strcmp(s_selection, "cpu-naive") == 0) {
        simulation_choice = CPU_NAIVE;
      } else if (std::strcmp(s_selection, "cpu-barnes-hut") == 0) {
        simulation_choice = CPU_BARNES_HUT;
      } else if (std::strcmp(s_selection, "gpu-naive") == 0) {
        simulation_choice = GPU_NAIVE;
      } else if (std::strcmp(s_selection, "gpu-barnes-hut") == 0) {
        simulation_choice = GPU_BARNES_HUT;
      } else {
        print_usage();
        exit(0);
      }
    }
  }

  {
    char* device;

    if (device = cmd_option_value("--opencl-device")) {
      opencl_device_choice = std::atoi(device);
    }
  }

  {
    char* bodies;

    if (bodies = cmd_option_value("--bodies")) {
      n_bodies = std::atoi(bodies);
    }
  }
}
