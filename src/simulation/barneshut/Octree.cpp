#include "simulation/barneshut/Octree.h"

Octree::Octree(Vec3 o, Vec3 b) {
  origin = o;
  half_dimension = b;
}

void Octree::insert(Particle* P) {
  if (!is_leaf()) {
    children[get_octane(P)]->insert(P);
    return;
  }

  if (particle == nullptr) {
    particle = P;
    return;
  }

  Particle* P_old = particle;
  particle = nullptr;

  for (int i = 0; i < 8; ++i) {
    Vec3 new_origin = origin;

    new_origin.x = half_dimension.x * (i&4 ? 0.5f : -0.5f);
    new_origin.y = half_dimension.y * (i&2 ? 0.5f : -0.5f);
    new_origin.z = half_dimension.z * (i&1 ? 0.5f : -0.5f);

    children[i] = new Octree(new_origin, half_dimension * 0.5f);
  }

  children[get_octane(P_old)]->insert(P_old);
  children[get_octane(P)]->insert(P);
}

bool Octree::is_leaf() {
  return children[0] != nullptr;
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
