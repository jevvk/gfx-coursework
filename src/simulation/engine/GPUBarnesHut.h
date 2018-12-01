#pragma once

#include "Particle.h"

class GPUBarnesHut {
public:
  GPUBarnesHut(Particle* particles);
  ~GPUBarnesHut();

  void forward();

private:
  Particle* particles;
};
