#pragma once

#include "universe/Universe.h"
#include "simulation/Simulation.h"

class CPUNaiveSimulation : public Simulation {
public:
  CPUNaiveSimulation(Universe* universe) : Simulation(universe, 200) {};

  void step();
  void render();

private:
  void forward();
  void calculate_forces();
  void forward_1();
  void forward_2();
  void swap_forces();

  static void draw_axes();
};
