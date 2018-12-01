#include <iostream>
#include <string>
#include <math.h>

#include <GL/glu.h>
#include <GL/glut.h>

#include "Vec3.h"
#include "Particle.h"
#include "simulation/CPUBarnesHutSimulation.h"

#define THRESHOLD_DISTANCE 1e-6

CPUBarnesHutSimulation::CPUBarnesHutSimulation(Universe* universe) : Simulation(universe, 2000) {
  barnes_hut = new BarnesHut(Vec3(ORIGIN_X, ORIGIN_Y, ORIGIN_Z), Vec3(BOX_X, BOX_Y, BOX_Z));
}

void CPUBarnesHutSimulation::step() {
  if (paused) {
    return;
  }

  forward();
}

void CPUBarnesHutSimulation::render() {
  // std::cout << "CPUNaiveSimuCPUBarnesHutSimulationlator: render" << std::endl; 

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt(camera->eyex, camera->eyey, camera->eyez,
            camera->centerx, camera->centery, camera->centerz,
            camera->upx, camera->upy, camera->upz);
  
  glPushMatrix();

  Particle::begin_render();

  for (int i = 0; i < n_particles; ++i) {
    particles[i].render();
  }

  Particle::end_render();

  glPopMatrix();

  // draw_axes();
}

void CPUBarnesHutSimulation::calculate_forces()  {
  barnes_hut->update_forces(particles, n_particles);
}

void CPUBarnesHutSimulation::forward_1() {
  for (int i = 0; i < n_particles; ++i) {
    Particle* P = &particles[i];

    Vec3 acc = P->force / P->mass;
    Vec3 vel = acc / 2 * time_step + P->vel;

    P->pos += vel * time_step;
  }
}

void CPUBarnesHutSimulation::forward_2() {
  for (int i = 0; i < n_particles; ++i) {
    Particle* P = &particles[i];

    Vec3 acc = P->force / P->mass;
    Vec3 _acc = P->_force / P->mass;

    P->vel += (acc + _acc) / 2 * time_step;
  }
}

void CPUBarnesHutSimulation::swap_forces() {
  for (int i = 0; i < n_particles; ++i) {
    Particle* P = &particles[i];

    P->_force = P->force;
  }
}

void CPUBarnesHutSimulation::forward() {
  for (int i = 0; i < n_particles; ++i) {
    Particle* P = &particles[i];

    P->force = Vec3(0, 0, 0);
  }

  // acceleration = force(time, position) / mass;
  // time += timestep;
  // position += timestep * (velocity + timestep * acceleration / 2);
  // newAcceleration = force(time, position) / mass;
  // velocity += timestep * (acceleration + newAcceleration) / 2;

  // assumes it's already calculated
  calculate_forces();
  forward_1();
  swap_forces();
  calculate_forces();
  forward_2();
}
