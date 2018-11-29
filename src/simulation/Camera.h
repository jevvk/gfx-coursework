#pragma once

class Camera {
public:
  Camera();

  void reset_acceleration();
  void increase_acceleration();

  void update_center();

  void look_up();
  void look_down();
  void look_left();
  void look_right();

  void go_forwards();
  void go_backwards();
  void go_left();
  void go_right();

  float eyex, eyey, eyez;
  float centerx, centery, centerz;
  float upx, upy, upz;

private:
  double acceleration;
  double lon, lat;
  double mlon, mlat;
};
