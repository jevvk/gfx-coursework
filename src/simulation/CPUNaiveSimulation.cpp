#include <iostream>
#include <string>
#include <math.h>

#include <GL/glu.h>
#include <GL/glut.h>

#include "Particle.h"
#include "simulation/CPUNaiveSimulation.h"

#define THRESHOLD_DISTANCE 0.00001

void CPUNaiveSimulation::step() {
  if (paused) {
    return;
  }

  forward();
}

void CPUNaiveSimulation::render() {
  // std::cout << "CPUNaiveSimulator: render" << std::endl; 

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt(camera->eyex, camera->eyey, camera->eyez,
            camera->centerx, camera->centery, camera->centerz,
            camera->upx, camera->upy, camera->upz);
  
  glPushMatrix();

  Particle::begin_render();

  for (int i = 0; i < N_PARTICLES; ++i) {
    particles[i].render();
  }

  Particle::end_render();

  glPopMatrix();

  // draw_axes();
}

void CPUNaiveSimulation::calculate_forces() {
  for (int i = 0; i < N_PARTICLES; ++i) {
    Particle* P1 = &particles[i];

    // if (!P1->alive) {
    //   continue;
    // }

    for (int j = i + 1; j < N_PARTICLES; ++j) {
      Particle* P2 = &particles[j];

      // if (!P2->alive) {
      //   continue;
      // }

      Vec3 diff = P1->pos - P2->pos;
      double sqrt_dist = diff.norm() + EPSILON;
      double force = (-gravity_const * P1->mass * P2->mass) / (sqrt_dist * sqrt_dist);

      Vec3 force3 = force * diff / sqrt_dist;

      P1->force += force3;
      P2->force -= force3;

      // if (sqrt_dist < P1->radius + P2->radius) {
      //   std::cout << "CPUNaiveSimulator: combining particles." << std::endl;

      //   P2->alive = false;

      //   double total_mass = P1->mass + P2->mass;

      //   P1->vel = (P1->vel * P1->mass + P2->vel * P2->mass) / total_mass;
      //   P1->force = (P1->force + P2->force) / 2;
      //   P1->mass = total_mass;

      //   // debug_particle(*P1, "Pt");
      // }
    }
  }

}

void CPUNaiveSimulation::forward_1() {
  for (int i = 0; i < N_PARTICLES; ++i) {
    Particle* P = &particles[i];

    Vec3 acc = P->force / P->mass;
    Vec3 vel = acc / 2 * time_step + P->vel;

    P->pos += vel * time_step;
  }
}

void CPUNaiveSimulation::forward_2() {
  for (int i = 0; i < N_PARTICLES; ++i) {
    Particle* P = &particles[i];

    Vec3 acc = P->force / P->mass;
    Vec3 _acc = P->_force / P->mass;

    P->vel += (acc + _acc) / 2 * time_step;
  }
}

void CPUNaiveSimulation::swap_forces() {
  for (int i = 0; i < N_PARTICLES; ++i) {
    Particle* P = &particles[i];

    P->_force = P->force;
  }
}

void CPUNaiveSimulation::forward() {
  for (int i = 0; i < N_PARTICLES; ++i) {
    Particle* P = &particles[i];

    P->force = Vec3(0, 0, 0);
  }

  // acceleration = force(time, position) / mass;
  // time += timestep;
  // position += timestep * (velocity + timestep * acceleration / 2);
  // newAcceleration = force(time, position) / mass;
  // velocity += timestep * (acceleration + newAcceleration) / 2;

  calculate_forces();
  forward_1();
  swap_forces();
  calculate_forces();
  forward_2();

}
