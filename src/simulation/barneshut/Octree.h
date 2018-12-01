#pragma once

#include "Vec3.h"
#include "Particle.h"

class Octree {
public:
  Octree(Vec3 origin, Vec3 half_dimension);
  Octree(const Octree& copy) : origin(copy.origin), half_dimension(copy.half_dimension), particle(copy.particle) {};
  ~Octree();

  Vec3 origin, half_dimension;
  Octree* children[8];
  Particle* particle;

  double total_mass;
  Vec3 mass_center;
  bool has_children;

  void insert(Particle* P);
  bool is_leaf();
  
  static void free_tree(Octree* node);

private:
  int get_octane(Particle* P);
};
