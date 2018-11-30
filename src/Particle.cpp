#include <math.h>
#include <GL/glut.h>

#include "Particle.h"

void Particle::begin_render() {
}

void Particle::render() {
  glColor3f(1.0, 1.0, 1.0);
  glPushMatrix();
  // glVertex3f(pos.x, pos.y, pos.z);
  glTranslatef(pos.x, pos.y, pos.z);
  glutWireSphere(radius, 5, 5);
  glPopMatrix();
}

void Particle::end_render() {
}
