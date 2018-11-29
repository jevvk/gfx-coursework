#pragma once

#include "Particle.h"

/**
 * This class is used to generate particles.
 **/
class Universe {
public:
  Universe() {};

  virtual Particle* generate(int N) { return 0; };
};
