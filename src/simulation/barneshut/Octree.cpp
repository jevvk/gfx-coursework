#include "simulation/barneshut/Octree.h"

Octree::Octree(Vec3 o, Vec3 b) {
  particle = NULL;
  origin = o;
  half_dimension = b;
  has_children = false;

  for (int i = 0; i < 8; ++i) {
    children[i] = NULL;
  }
}

Octree::~Octree() {
  for (int i = 0; i < 8; ++i) {
    delete children[i];
  }
}

int count = 0;

void Octree::insert(Particle* P) {
  if (has_children) {
    children[get_octane(P)]->insert(P);
    return;
  }

  if (particle == NULL) {
    particle = P;
    return;
  }

  Particle* old_particle = particle;
  particle = NULL;

  for (int i = 0; i < 8; ++i) {
    Vec3 new_origin = origin;

    new_origin.x += half_dimension.x * (i&4 ? 0.5f : -0.5f);
    new_origin.y += half_dimension.y * (i&2 ? 0.5f : -0.5f);
    new_origin.z += half_dimension.z * (i&1 ? 0.5f : -0.5f);

    children[i] = new Octree(new_origin, half_dimension * 0.5f);
  }

  has_children = true;

  insert(old_particle);
  insert(P);
}

bool Octree::is_leaf() {
  return !has_children;
}

int Octree::get_octane(Particle* P) {
  int oct = 0;

  if (P->pos.x >= origin.x) oct |= 4;
  if (P->pos.y >= origin.y) oct |= 2;
  if (P->pos.z >= origin.z) oct |= 1;

  return oct;
}

void Octree::free_tree(Octree* node) {
  if (!node->is_leaf()) {
    for (int i = 0; i < 8; ++i) {
      free_tree(node->children[i]);
    }
  }

  free(node);
}
