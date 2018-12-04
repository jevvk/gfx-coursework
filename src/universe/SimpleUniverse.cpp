#include <math.h>
#include <iostream>

#include "Vec3.h"
#include "universe/Universe.h"

#define UNIVERSE_RADIUS 300.0
#define RADIUS_MINIMUM 0.2
#define RADIUS_VARIANCE 0.15
#define VEL_VARIANCE 20.5
#define MASS_VARIANCE 3.5e12
#define MASS_MINIMUM 5.0e10

#define X_SCALE 1.0
#define Y_SCALE 1.0 / 60
#define Z_SCALE 1.0

static double frand() {
  return ((double) rand()) / RAND_MAX;
}

/**
 * Places N particles randomly inside a sphere, with one large particle in the middle.
 **/
class SimpleUniverse : public Universe {
public:
  Vec3 camera_position() {
    return Vec3(1, UNIVERSE_RADIUS, UNIVERSE_RADIUS);
  }

  Vec3 camera_center() {
    return Vec3(0, 0, 0);
  }

  Particle* generate(int N) {
    std::cout << "SimpleUniverse: Generating " << N << " particles." << std::endl;
    std::cout << "SimpleUniverse: velocity variance (" << VEL_VARIANCE << "), mass VARIANCE (" << MASS_VARIANCE << ")" << std::endl;

    Particle* particles = new Particle[N];
    double max_dist = std::max(UNIVERSE_RADIUS * X_SCALE, std::max(UNIVERSE_RADIUS * Y_SCALE, UNIVERSE_RADIUS * Z_SCALE));

    // Particle* black_hole = &particles[0];

    // black_hole->pos.x = 0.1;
    // black_hole->pos.y = 0.1;
    // black_hole->pos.z = 0;
    // black_hole->vel.x = 0;
    // black_hole->vel.y = 0;
    // black_hole->vel.z = 0;
    // black_hole->mass = MASS_MINIMUM * N * 5;
    // black_hole->radius = RADIUS_MINIMUM + frand() * RADIUS_VARIANCE;
    // black_hole->alive = true;

    for (int i = 0; i < N; ++i) {
      Particle* P = &particles[i];

      float radius = (1 + frand()) / 2 * UNIVERSE_RADIUS;
      float z = 2 * frand() - 1; 
      float sqrtz = sqrt(1 - z * z);
      float t = 2 * M_PI * frand();

      P->pos.x = sqrtz * cos(t) * radius * X_SCALE;
      P->pos.y = sqrtz * sin(t) * radius * Y_SCALE;
      P->pos.z = z * radius * Z_SCALE;

      P->radius = RADIUS_MINIMUM + frand() * RADIUS_VARIANCE;
      P->mass = MASS_MINIMUM + frand() * MASS_VARIANCE;
      P->alive = true;

      Vec3 tmp = P->pos.normalized();

      // rotate 90 degrees
      P->vel.x = tmp.z;
      P->vel.y = 0;
      P->vel.z = -tmp.x;
      P->vel = P->vel * log(P->mass / P->pos.norm() * P->pos.norm()) / 4;
    }

    return particles;
  }
};
