#pragma once

#include "Vec3.h"
#include "Particle.h"

class Octree {
public:
  Octree(Vec3 origin, Vec3 half_dimension);

  Vec3 origin, half_dimension;
  Octree* children[8];
  Particle* particle;

  double total_mass;
  Vec3 mass_center;

  void insert(Particle* P);
  bool is_leaf();
  
  static void free_tree(Octree* node);

private:
  int get_octane(Particle* P);
};
