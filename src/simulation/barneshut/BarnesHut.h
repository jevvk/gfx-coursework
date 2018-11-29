#pragma once

#include "Particle.h"
#include "simulation/barneshut/Octree.h"

class BarnesHut {
public:
  BarnesHut(Vec3 origin, Vec3 box);
  ~BarnesHut();

  Vec3 origin, box;

  void update_forces(Particle* particles, int n);

private:
  Octree* tree;

  void _update_forces(Particle* particle, Octree* node);
  void _calculate_force(Particle* particle, Octree* node, double dist);

  double calculate_mass(Octree* node);
  Vec3 calculate_mass_center(Octree* node);
};
