#ifndef ROCKET_H
#define ROCKET_H



#include <math.h>
#include <stddef.h>
#include <stdlib.h>



#define ROCKET_EDGES_NUM 4


typedef struct {

  float x;
  float y;

  float des[ROCKET_EDGES_NUM << 1];
  float edges[ROCKET_EDGES_NUM << 1];

  float velx;
  float vely;
  float velmax;

  float angle;

} rocket_t;




rocket_t rocket;


void init_rocket(int window_width, int window_height) {
  rocket.x = window_width  >> 1;
  rocket.y = window_height >> 1;
  const float scaler = 4;
  rocket.des[(0 << 1) + 0] =  0 * scaler;
  rocket.des[(0 << 1) + 1] = -2 * scaler;
  rocket.des[(1 << 1) + 0] = +2 * scaler;
  rocket.des[(1 << 1) + 1] = +2 * scaler;
  rocket.des[(2 << 1) + 0] =  0 * scaler;
  rocket.des[(2 << 1) + 1] = +1 * scaler;
  rocket.des[(3 << 1) + 0] = -2 * scaler;
  rocket.des[(3 << 1) + 1] = +2 * scaler;
  for (int i = 0; i < ROCKET_EDGES_NUM << 1; ++i) {
    rocket.edges[i] = rocket.des[i];
  }

  rocket.angle = 0;
  rocket.velx = 0;
  rocket.vely = 0;
  rocket.velmax = 5;

}


void rotate_rocket(float dx, float dy) {
  const float velr = 0.2f;

  float angle = atan2(dx, -dy);
  if (angle < 0) angle = 3.141592 * 2 + angle;

  if (fabs(angle - rocket.angle) < velr) return;

  const int dir = angle < rocket.angle ^ fabs(angle - rocket.angle) > 3.141592 ? -1 : +1;
  rocket.angle += velr * dir;

  if      (rocket.angle < 0           ) rocket.angle = 3.141592 * 2 + rocket.angle;
  else if (rocket.angle > 3.141592 * 2) rocket.angle = rocket.angle - 3.141592 * 2;

  const float cosa = cos(rocket.angle);
  const float sina = sin(rocket.angle);
  for (int i = 0; i < ROCKET_EDGES_NUM; ++i) {
    rocket.edges[(i << 1) + 0] = rocket.des[(i << 1) + 0] * cosa - rocket.des[(i << 1) + 1] * sina;
    rocket.edges[(i << 1) + 1] = rocket.des[(i << 1) + 1] * cosa + rocket.des[(i << 1) + 0] * sina;
  }
}


void move_rocket(float accspeed, int window_width, int window_height) {
  float velmag = rocket.velx * rocket.velx + rocket.vely * rocket.vely;

    const float acc = 1.f * accspeed;
    const float dx = cos(rocket.angle) * acc;
    const float dy = sin(rocket.angle) * acc;

      rocket.velx += fmin(dx, rocket.velmax);
      rocket.vely += fmin(dy, rocket.velmax);

    // velocity cant be same direction as delta if we are not moving
    if (accspeed < 0 && rocket.velx * dx >= 0) rocket.velx = 0;
    if (accspeed < 0 && rocket.vely * dy >= 0) rocket.vely = 0;

  rocket.x += rocket.velx;
  rocket.y += rocket.vely;

  if      (rocket.x < 0            ) rocket.x = window_width + rocket.x;
  else if (rocket.x > window_width ) rocket.x = rocket.x - window_width;
  if      (rocket.y < 0            ) rocket.y = window_width + rocket.y;
  else if (rocket.y > window_height) rocket.y = rocket.y - window_width;
}


#endif // ROCKET_H
