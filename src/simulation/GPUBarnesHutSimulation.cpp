#include <GL/glu.h>
#include <GL/glut.h>

#include "universe/Universe.h"
#include "simulation/Simulation.h"

#include "simulation/GPUBarnesHutSimulation.h"

GPUBarnesHutSimulation::GPUBarnesHutSimulation(Universe* universe) : Simulation(universe, 20000) {
  barneshut = new GPUBarnesHut(particles);
}

void GPUBarnesHutSimulation::step() {
  if (paused) {
    return;
  }

  // forward();
}
