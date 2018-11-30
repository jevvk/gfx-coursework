#include <math.h>
#include <iostream>

#include "simulation/Camera.h"

#define DEG_TO_RAD M_PI / 180
#define TURN_ANGLE 4.0 * DEG_TO_RAD
#define RUN_SPEED 0.5
#define MAX_ACCEL 20
#define MUL_ACCEL 1.05

Camera::Camera() {
  acceleration = 1.0;

  mlon = 0.0;
  mlat = 0.0;

  eyex = 0.0; // probably have to change these 3
  eyey = 200.0; // maybe use universe to set these
  eyez = 50.0;

  centerx = 0.0;
  centery = 0.0;
  centerz = 0.0;

  upx = 0.0;
  upy = 1.0;
  upz = 0.0;

  float dist = sqrt(eyex * eyex + eyey * eyey + eyez * eyez);

  lon = atan2(eyex - centerx, eyey - centery);
  lat = asin((eyey - centery) / dist);
}

void Camera::reset_acceleration() {
  acceleration /= MUL_ACCEL;
}

void Camera::increase_acceleration() {
  acceleration *= MUL_ACCEL;
  if (acceleration > MAX_ACCEL) acceleration = MAX_ACCEL;
}

void Camera::update_center() {
  float LON = lon;
  float LAT = lat;

  if (LAT > M_PI_2) LAT = M_PI_2;
  if (LAT <= -M_PI_2) LAT = -M_PI_2 - 1e-3;

  float clat = cos(LAT);
  float slat = sin(LAT);
  float clon = cos(LON);
  float slon = sin(LON);

  centerx = eyex - clat * slon;
  centery = eyey - slat;
  centerz = eyez - clat * clon;
}

void Camera::look_up() {
  lat -= TURN_ANGLE;
  if (lat <= -M_PI_2) lat = -M_PI_2 - 1e-3;
}

void Camera::look_down() {
  lat += TURN_ANGLE;
  if (lat > M_PI_2) lat = M_PI_2;
}

void Camera::look_left() {
  lon += TURN_ANGLE;
}

void Camera::look_right() {
  lon -= TURN_ANGLE;
}

void Camera::go_forwards() {
  const float slon = sin(lon);
  const float clon = cos(lon);
  const float slat = sin(lat);
  const float clat = cos(lat);
  const float run_speed = RUN_SPEED * acceleration;

  eyex -= clat * slon * run_speed;
  eyey -= slat * run_speed;
  eyez -= clat * clon * run_speed;
}

void Camera::go_backwards() {
  const float slon = sin(lon);
  const float clon = cos(lon);
  const float slat = sin(lat);
  const float clat = cos(lat);
  const float run_speed = RUN_SPEED * acceleration;

  eyex += clat * slon * run_speed;
  eyey += slat * run_speed;
  eyez += clat * clon * run_speed;
}

void Camera::go_left() {
  const float run_speed = RUN_SPEED * acceleration;

  eyex += cos(lat * DEG_TO_RAD) * sin(lon * DEG_TO_RAD - M_PI / 2) * run_speed;
  eyez += cos(lat * DEG_TO_RAD) * cos(lon * DEG_TO_RAD - M_PI / 2) * run_speed;
}

void Camera::go_right() {
  const float run_speed = RUN_SPEED * acceleration;

  eyex -= cos(lat * DEG_TO_RAD) * sin(lon * DEG_TO_RAD - M_PI / 2) * run_speed;
  eyez -= cos(lat * DEG_TO_RAD) * cos(lon * DEG_TO_RAD - M_PI / 2) * run_speed;
}
