#pragma once

enum UniverseChoice { RANDOM_SPHERE, TWIN_COLLISION };
enum SimulationChoice { CPU_NAIVE, CPU_BARNES_HUT, GPU_BARNES_HUT };

extern UniverseChoice universe_choice;
extern SimulationChoice simulation_choice;
extern int opencl_device_choice;
extern int n_bodies;

void get_options(int argc, char * argv[]);
