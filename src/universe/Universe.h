#pragma once

#include "Particle.h"
#include "Vec3.h"

/**
 * This class is used to generate particles.
 **/
class Universe {
public:
  Universe() {};

  virtual Particle* generate(int N) { return 0; };
  virtual Vec3 camera_position() { return Vec3(0, 0, 0); };
  virtual Vec3 camera_center() { return Vec3(0, 0, 0); };
};
