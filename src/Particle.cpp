#include <GL/glut.h>

#include "Particle.h"

void Particle::begin_render() {
  glBegin(GL_POINTS);
}

void Particle::render() {
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(pos.x, pos.y, pos.z);
}

void Particle::end_render() {
  glEnd();
}
