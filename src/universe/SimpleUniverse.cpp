#include <math.h>
#include <iostream>

#include "universe/Universe.h"

#define RADIUS 50.0
#define VEL_DEVIATION 1.5
#define MASS_DEVIATION 3.5e10
#define MASS_MINIMUM 5.0e10

#define X_SCALE 1.0
#define Y_SCALE 1.0 / 60
#define Z_SCALE 1.0 / 2.5

static double frand() {
  return ((double) rand()) / RAND_MAX;
}

/**
 * Places N particles randomly inside a sphere, with one large particle in the middle.
 **/
class SimpleUniverse : public Universe {
public:
  Particle* generate(int N) {
    std::cout << "SimpleUniverse: Generating " << N << " particles." << std::endl;
    std::cout << "SimpleUniverse: velocity deviation (" << VEL_DEVIATION << "), mass deviation (" << MASS_DEVIATION << ")" << std::endl;

    Particle* particles = new Particle[N];
    Particle* star = &particles[0];

    star->mass = MASS_MINIMUM * log(N);
    star->pos.x = 0.0;
    star->pos.y = 0.0;
    star->pos.z = -0.5 * RADIUS * Z_SCALE;
    star->vel.x = 0;
    star->vel.y = 0.0;
    star->vel.z = 0.0;
    star->alive = true;

    star = &particles[1];

    star->mass = MASS_MINIMUM * log(N);
    star->pos.x = 0.0;
    star->pos.y = 0.0;
    star->pos.z = 0.5 * RADIUS * Z_SCALE;
    star->vel.x = 0.1;
    star->vel.y = 0.0;
    star->vel.z = 0.0;
    star->alive = true;

    for (int i = 2; i < N; ++i) {
      Particle* P = &particles[i];

      float radius = (1 + frand()) / 2 * RADIUS;
      float z = 2 * frand() - 1;
      float sqrtz = sqrt(1 - z * z);
      float t = 2 * M_PI * frand();

      P->pos.x = sqrtz * cos(t) * radius * X_SCALE;
      P->pos.y = sqrtz * sin(t) * radius * Y_SCALE;
      P->pos.z = z * radius * Z_SCALE;

      P->mass = MASS_MINIMUM + frand() * MASS_DEVIATION;
      P->alive = true;

      P->vel.x = (frand() * VEL_DEVIATION - VEL_DEVIATION / 2) * X_SCALE;
      P->vel.y = (frand() * VEL_DEVIATION - VEL_DEVIATION / 2) * Y_SCALE;
      P->vel.z = (frand() * VEL_DEVIATION - VEL_DEVIATION / 2) * Z_SCALE;
    }

    return particles;
  }
};
