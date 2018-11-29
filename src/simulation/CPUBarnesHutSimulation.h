#pragma once

#include "universe/Universe.h"
#include "simulation/Simulation.h"
#include "simulation/barneshut/BarnesHut.h"

#define ORIGIN_X 0
#define ORIGIN_Y 0
#define ORIGIN_Z 0

#define BOX_X 1000000.0
#define BOX_Y 1000000.0
#define BOX_Z 1000000.0

class CPUBarnesHutSimulation : public Simulation {
public:
  CPUBarnesHutSimulation(Universe* universe);

  void step();
  void render();

private:
  BarnesHut* barnes_hut;

  void forward();
  void calculate_forces();
  void forward_1(double time_step);
  void forward_2(double time_step);
  void swap_forces();

  static void draw_axes();
};
