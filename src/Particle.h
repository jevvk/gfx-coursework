#pragma once

#include "Vec3.h"

class Particle {
public:
  Vec3 pos, vel, force, _force;
  float mass;
  bool alive;

  void render();

  static void begin_render();
  static void end_render();
};
