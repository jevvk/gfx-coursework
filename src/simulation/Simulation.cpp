#include <math.h>
#include <iostream>

#include <GL/glut.h>

#include "simulation/Simulation.h"

Simulation::Simulation(Universe* u, int particle_count) {
  universe = u;
  paused = false;
  n_particles = particle_count;
  particles = universe->generate(n_particles);
  camera = new Camera(universe->camera_position(), universe->camera_center());
}

void Simulation::reset() {
  std::cout << "CPUNaiveSimulator: resetting simulation." << std::endl;

  _reset();
  free(particles);

  particles = universe->generate(n_particles);
  paused = false;
}

void Simulation::handle_key(unsigned char key) {
  // std::cout << "Simulation: handle key " << key << std::endl;

  if (last_key != key) {
    camera->reset_acceleration();
  }

  switch (key) {
  case 'w':
    camera->go_forwards();
    camera->increase_acceleration();
    break;
  case 's':
    camera->go_backwards();
    camera->increase_acceleration();
    break;
  case 'a':
    camera->go_left();
    camera->increase_acceleration();
    break;
  case 'd':
    camera->go_right();
    camera->increase_acceleration();
    break;
  
  case 'r':
    reset();
    break;
  case 'p':
    paused = !paused;
    break;

  case 'g':
    gravity_const += 1.0e-12;
    std::cout << "Simulation: gravity constant change " << gravity_const << std::endl;
    break;
  case 'G':
    gravity_const -= 1.0e-12;
    std::cout << "Simulation: gravity constant change " << gravity_const << std::endl;
    break;

  case 't':
    time_step += 0.025;
    std::cout << "Simulation: time step change " << time_step << std::endl;
    break;
  case 'T':
    time_step -= 0.025;
    std::cout << "Simulation: time step change " << time_step << std::endl;
    break;

  default:
    _handle_key(key);
    break;
  }

  camera->update_center();
  last_key = key;
}

void Simulation::handle_specialkey(int key) {
  switch (key) {
  case GLUT_KEY_UP:
    camera->look_up();
    break;
  case GLUT_KEY_DOWN:
    camera->look_down();
    break;
  case GLUT_KEY_LEFT:
    camera->look_left();
    break;
  case GLUT_KEY_RIGHT:
    camera->look_right();
    break;

  default:
    _handle_specialkey(key);
    break;
  }

  camera->update_center();
}

void Simulation::debug_particles() {
  debug_particle(particles[0], "1");
  debug_particle(particles[5], "6");

  Particle P1 = particles[0];
  Particle P2 = particles[5];

  double d_x = P1.pos.x - P2.pos.x;
  double d_y = P1.pos.y - P2.pos.y;
  double d_z = P1.pos.z - P2.pos.z;

  double distance = sqrt(d_x * d_x + d_y * d_y + d_z * d_z) + EPSILON;
  double force = (gravity_const * P1.mass * P2.mass) / (distance * distance);

  std::cout << "Simulation: distance " << distance << " force " << force << std::endl;
}

void Simulation::debug_particle(Particle P, const char* name) {
  std::cout << "Simulation: particle " << name << " ";
  std::cout << P.pos.x << " " << P.pos.y << " " << P.pos.z << " |m ";
  std::cout << P.mass << " |v ";
  std::cout << P.vel.x << " " << P.vel.y << " " << P.vel.z << " |f ";
  std::cout << P.force.x << " " << P.force.y << " " << P.force.z << std::endl;
}

void Simulation::debug_camera() {
  std::cout << "Simulation: camera ";
  std::cout << camera->eyex << " " << camera->eyey << " " << camera->eyez << " ";
  std::cout << camera->centerx << " " << camera->centery << " " << camera->centerz << std::endl;
}

void Simulation::draw_axes() {
  const float UNIT = 1000.0;

  glPushMatrix();
  glBegin(GL_LINES);

  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(UNIT, 0.0, 0.0);

  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, UNIT, 0.0);

  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, UNIT);

  glEnd();
  glPopMatrix();
}

