#include <iostream>
#include <string>
#include <math.h>

#include <GL/glu.h>
#include <GL/glut.h>

#include "Particle.h"
#include "simulation/CPUNaiveSimulation.h"

#define THRESHOLD_DISTANCE 1e-6

void CPUNaiveSimulation::step() {
  if (paused) {
    return;
  }

  calculate_forces();

  // do multiple samples for better accuracy, but worse performance
  for (int i = 0; i < n_samples; ++i) {
    forward();
  }
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

    if (!P1->alive) {
      continue;
    }

    for (int j = i + 1; j < N_PARTICLES; ++j) {
      Particle* P2 = &particles[j];

      if (!P2->alive) {
        continue;
      }

      Vec3 diff = P1->pos - P2->pos;
      double sqrt_dist = diff.norm();
      double force = sqrt_dist > EPSILON ? (gravity_const * P1->mass * P2->mass) / (sqrt_dist * sqrt_dist) : 0;

      Vec3 force3 = force * (P1->pos - P2->pos) / sqrt_dist;

      P1->force -= force3;
      P2->force += force3;

      // double forcex = force * (P1->pos.x - P2->pos.x) / sqrt_dist;
      // P1->force.x -= forcex;
      // P2->force.x += forcex;

      // double forcey = force * (P1->pos.y - P2->pos.y) / sqrt_dist;
      // P1->force.y -= forcey;
      // P2->force.y += forcey;

      // double forcez = force * (P1->pos.z - P2->pos.z) / sqrt_dist;
      // P1->force.z -= forcez;
      // P2->force.z += forcez;

      if (sqrt_dist < THRESHOLD_DISTANCE) {
        // std::cout << "CPUNaiveSimulator: combining particles." << std::endl;
        // debug_particle(*P1, std::to_string(i).c_str());
        // debug_particle(*P2, std::to_string(j).c_str());

        P2->alive = false;

        double total_mass = P1->mass + P2->mass;

        P1->vel = (P1->vel * P1->mass + P2->vel * P2->mass) / total_mass;
        P1->force = (P1->force + P2->force) / 2;
        P1->mass = total_mass;

        // P1->vel.x = (P1->vel.x * P1->mass + P2->vel.x * P2->mass) / total_mass;
        // P1->vel.y = (P1->vel.y * P1->mass + P2->vel.y * P2->mass) / total_mass;
        // P1->vel.z = (P1->vel.z * P1->mass + P2->vel.z * P2->mass) / total_mass;

        // P1->forcex = (P1->forcex + P2->forcex) / 2;
        // P1->forcey = (P1->forcey + P2->forcey) / 2;
        // P1->forcez = (P1->forcez + P2->forcez) / 2;

        // debug_particle(*P1, "Pt");
      }
    }
  }
}

void CPUNaiveSimulation::forward_1(double _time_step) {
  for (int i = 0; i < N_PARTICLES; ++i) {
    Particle* P = &particles[i];

    Vec3 acc = P->force / P->mass;
    Vec3 vel = acc / 2 * _time_step + P->vel;

    P->pos += vel * _time_step;
  }
}

void CPUNaiveSimulation::forward_2(double _time_step) {
  for (int i = 0; i < N_PARTICLES; ++i) {
    Particle* P = &particles[i];

    Vec3 acc = P->force / P->mass;
    Vec3 _acc = P->_force / P->mass;

    P->vel += (acc + _acc) / 2 * _time_step;
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

  // 2 passes
  double _time_step = ((double) time_step) / n_samples;

  // calculate_forces(); // might be able to skip this
  forward_1(_time_step);
  swap_forces();
  calculate_forces();
  forward_2(_time_step);

}
