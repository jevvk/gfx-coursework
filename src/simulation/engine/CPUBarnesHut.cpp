#pragma once
#include <iostream>
#include <algorithm>

#include "simulation/engine/CPUBarnesHut.h"

#include "simulation/Simulation.h"

CPUBarnesHut::CPUBarnesHut(Vec3 o, Vec3 b) {
  origin = o;
  box = b;
}

CPUBarnesHut::~CPUBarnesHut() {
  delete tree;
}

void CPUBarnesHut::update_forces(Particle* particles, int n) {
  if (tree != NULL) {
    delete tree;
  }

  tree = new Octree(origin, box);
  
  for (int i = 0; i < n; ++i) {
    tree->insert(&particles[i]);
  }

  calculate_mass(tree);
  calculate_mass_center(tree);

  for (int i = 0; i < n; ++i) {
    Particle* P  = &particles[i];

    Vec3 min = origin - box / 2;
    Vec3 max = origin + box / 2;

    if ((min - P->pos).maxComponent() > 0 || (max - P->pos).minComponent() < 0) {
      P->alive = false;
    }
  }

  for (int i = 0; i < n; ++i) {
    _update_forces(&particles[i], tree);
  }
}

void CPUBarnesHut::_update_forces(Particle* particle, Octree* node) {
  if (!particle->alive) {
    return;
  }

  if (node->particle != NULL && node->particle != particle) {
    double dist = (particle->pos - node->mass_center).norm() + EPSILON;
    _calculate_force(particle, node, dist);
  } else if (!node->is_leaf()) {
    double dist = (particle->pos - node->mass_center).norm();
    double theta = std::max(node->half_dimension.x, std::max(node->half_dimension.y, node->half_dimension.z)) / dist;

    if (theta < SD_TRESHOLD) {
      _calculate_force(particle, node, dist + EPSILON);
      return;
    }
    
    for (int i = 0; i < 8; ++i) {
      _update_forces(particle, node->children[i]);
    }
  }
}

void CPUBarnesHut::_calculate_force(Particle* particle, Octree* node, double dist) {
  double force = -GRAVITY_CONST * particle->mass * node->total_mass / (dist * dist);
  particle->force += force * (particle->pos - node->mass_center) / dist;
}

double CPUBarnesHut::calculate_mass(Octree* node) {
  if (!node->is_leaf()) {
    node->total_mass = 0;

    for (int i = 0; i < 8; ++i) {
      node->total_mass += calculate_mass(node->children[i]);
    }
  } else if (node->particle != NULL) {
    node->total_mass = node->particle->mass;
  } else {
    node->total_mass = 0;
  }

  return node->total_mass;
}

Vec3 CPUBarnesHut::calculate_mass_center(Octree* node) {
  if (!node->is_leaf()) {
    node->mass_center = Vec3(0, 0, 0);
    double mass_total = 0;

    for (int i = 0; i < 8; ++i) {
      if (node->is_leaf() && node->particle == NULL) {
        continue;
      }

      node->mass_center += calculate_mass_center(node->children[i]) * node->children[i]->total_mass;
      mass_total += node->children[i]->total_mass;
    }

    node->mass_center = node->mass_center / mass_total;
  } else if (node->particle != NULL) {
    node->mass_center = node->particle->pos;
  } else {
    node->mass_center = Vec3(0, 0, 0);
  }

  return node->mass_center;
}
