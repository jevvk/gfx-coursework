#pragma once

#include "universe/Universe.h"
#include "simulation/Simulation.h"

class CPUNaiveSimulation : public Simulation {
public:
  CPUNaiveSimulation(Universe* universe) : Simulation(universe) {};

  void step();
  void render();

private:
  void forward();
  void calculate_forces();
  void forward_1(double time_step);
  void forward_2(double time_step);
  void swap_forces();

  static void draw_axes();
};
