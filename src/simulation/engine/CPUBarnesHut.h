#pragma once

#include "Particle.h"
#include "simulation/engine/Octree.h"

#define SD_TRESHOLD 1.5

class CPUBarnesHut {
public:
  CPUBarnesHut(Vec3 origin, Vec3 box);
  ~CPUBarnesHut();

  Vec3 origin, box;

  void update_forces(Particle* particles, int n);

private:
  Octree* tree = NULL;

  void _update_forces(Particle* particle, Octree* node);
  void _calculate_force(Particle* particle, Octree* node, double dist);

  double calculate_mass(Octree* node);
  Vec3 calculate_mass_center(Octree* node);
};
