#pragma once

#include "simulation/barneshut/BarnesHut.h"

#include "simulation/Simulation.h"

BarnesHut::BarnesHut(Vec3 o, Vec3 b) {
  origin = o;
  box = b;
}

BarnesHut::~BarnesHut() {
  Octree::free_tree(tree);
}

void BarnesHut::update_forces(Particle* particles, int n) {
  if (tree != nullptr) {
    Octree::free_tree(tree);
  }

  tree = new Octree(origin, box);
  
  for (int i = 0; i < n; ++i) {
    tree->insert(&particles[i]);
  }

  for (int i = 0; i < n; ++i) {
    _update_forces(&particles[i], tree);
  }
}

void BarnesHut::_update_forces(Particle* particle, Octree* node) {
  if (node->particle != nullptr && node->particle != particle) {
    double dist = (particle->pos - node->mass_center).norm();
    _calculate_force(particle, node, dist);
  } else if (!node->is_leaf()) {
    double dist = (particle->pos - node->mass_center).norm();
    double theta = node->half_dimension.x * 2 / dist; // why x?

    if (theta < 0.5) {
      _calculate_force(particle, node, dist);
      return;
    }
    
    for (int i = 0; i < 8; ++i) {
      _update_forces(particle, node->children[i]);
    }
  }
}

void BarnesHut::_calculate_force(Particle* particle, Octree* node, double dist) {
  double force = dist > EPSILON ? -GRAVITY_CONST * particle->mass * node->total_mass / (dist * dist) : 0;
  particle->force += force * (particle->pos - node->mass_center);
}

double BarnesHut::calculate_mass(Octree* node) {
  if (!node->is_leaf()) {
    node->total_mass = 0;

    for (int i = 0; i < 8; ++i) {
      node->total_mass += calculate_mass(node->children[i]);
    }
  } else if (node->particle != nullptr) {
    node->total_mass = node->particle->mass;
  } else {
    node->total_mass = 0;
  }

  return node->total_mass;
}

Vec3 BarnesHut::calculate_mass_center(Octree* node) {
  if (!node->is_leaf()) {
    node->mass_center = Vec3(0, 0, 0);
    double mass_total = 0;

    for (int i = 0; i < 8; ++i) {
      if (node->is_leaf() && node->particle == nullptr) {
        continue;
      }

      node->mass_center += calculate_mass_center(node->children[i]) * node->children[i]->total_mass;
      mass_total += node->children[i]->total_mass;
    }

    node->mass_center = node->mass_center / mass_total;
  } else if (node->particle != nullptr) {
    node->mass_center = node->particle->pos;
  } else {
    node->mass_center = Vec3(0, 0, 0);
  }

  return node->mass_center;
}
