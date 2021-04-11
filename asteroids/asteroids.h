#ifndef ASTEROIDS_H
#define ASTEROIDS_H


#include <math.h>
#include <stddef.h>
#include <stdlib.h>


#define ASTEROID_EDGES_NUM 6
#define ASTEROIDS_NUM      3


typedef struct {

  float des[ASTEROID_EDGES_NUM << 1]; // default edges
  float edges[ASTEROID_EDGES_NUM << 1];

  float x;
  float y;

  float mass;

  float velx;
  float vely;
  float velr;

  float radius;

  float angle;

  int is_dead;

} asteroid_t;




asteroid_t asteroids[ASTEROIDS_NUM];


void init_asteroid(asteroid_t *a, int window_width, int window_height) {

  (*a).radius = 20;

  for (int i = 0; i < ASTEROID_EDGES_NUM; ++i) {
    const float radius = ((*a).radius + rand() % ((int)(*a).radius >> 1) - (*a).radius / 2);
    const float angle = 3.141592f * 2.f / ASTEROID_EDGES_NUM * i;
    (*a).des[(i << 1) + 0] = radius * cos(angle);
    (*a).des[(i << 1) + 1] = radius * sin(angle);
    (*a).edges[(i << 1) + 0] = (*a).des[(i << 1) + 0];
    (*a).edges[(i << 1) + 1] = (*a).des[(i << 1) + 1];
  }

  (*a).x = window_width  + (*a).radius / 4;
  (*a).y = window_height + (*a).radius / 4;
  switch(rand() % 4) {
  case 0: (*a).y = 0;
  case 1: (*a).x = rand() % (int)(*a).x; break;
  case 2: (*a).x = 0;
  case 3: (*a).y = rand() % (int)(*a).y; break;
  }
  (*a).x -= (*a).radius / 2;
  (*a).y -= (*a).radius / 2;

  int center_offset = 50;
  int center_x = (window_width  >> 1) + rand() % center_offset - (center_offset >> 1);
  int center_y = (window_height >> 1) + rand() % center_offset - (center_offset >> 1);

  float dx = center_x - (*a).x;
  float dy = center_y - (*a).y;
  float mag = sqrt(dx * dx + dy * dy);
  float speed = 5 + rand() % 10 / 5.f;
  (*a).velx = dx / mag * speed;
  (*a).vely = dy / mag * speed;
  (*a).velr = 0.1f * (rand() % 2 ? -1 : +1);

  (*a).is_dead = 0;
  (*a).mass    = 1.f;
  (*a).angle   = 0.f;
}



void rotate_asteroid(asteroid_t *a) {
  (*a).angle += (*a).velr;
  const float cosa = cos((*a).angle);
  const float sina = sin((*a).angle);
  for (int i = 0; i < ASTEROID_EDGES_NUM; ++i) {
    (*a).edges[(i << 1) + 0] = (*a).des[(i << 1) + 0] * cosa - (*a).des[(i << 1) + 1] * sina;
    (*a).edges[(i << 1) + 1] = (*a).des[(i << 1) + 1] * cosa + (*a).des[(i << 1) + 0] * sina;
  }
}



// Returns 1 if the lines intersect, otherwise 0. In addition, if the lines
// intersect the intersection point may be stored in the floats i_x and i_y.
char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y,
                           float p2_x, float p2_y, float p3_x, float p3_y,
                           float *i_x, float *i_y)
{
  const float s1_x = p1_x - p0_x;
  const float s2_x = p3_x - p2_x;
  const float s1_y = p1_y - p0_y;
  const float s2_y = p3_y - p2_y;
  const float s2s1 = s1_x * s2_y - s2_x * s1_y;
  const float s = (s1_x * (p0_y - p2_y) - s1_y * (p0_x - p2_x)) / s2s1;
  const float t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / s2s1;
  if (i_x != NULL) *i_x = p0_x + (t * s1_x);
  if (i_y != NULL) *i_y = p0_y + (t * s1_y);
  return (s >= 0 && s <= 1 && t >= 0 && t <= 1);
}




int proc_asteroids_collision(int i) {
  #define TEST_AB(x, y, x1, y1, x2, y2) x1 <= x && x <= x2 && y1 <= y && y <= y2

  asteroid_t *a = &asteroids[i];

  for (int t = 0; t < ASTEROIDS_NUM; ++t) {
    if (t == i) continue;

    asteroid_t *b = &asteroids[t];

    const int TL =
      TEST_AB((*a).x - (*a).radius, (*a).y - (*a).radius,
              (*b).x - (*b).radius, (*b).y - (*b).radius,
              (*b).x + (*b).radius, (*b).y + (*b).radius);

    const int TR =
      TEST_AB((*a).x + (*a).radius, (*a).y - (*a).radius,
              (*b).x - (*b).radius, (*b).y - (*b).radius,
              (*b).x + (*b).radius, (*b).y + (*b).radius);

    const int BL =
      TEST_AB((*a).x - (*a).radius, (*a).y + (*a).radius,
              (*b).x - (*b).radius, (*b).y - (*b).radius,
              (*b).x + (*b).radius, (*b).y + (*b).radius);

    const int BR =
      TEST_AB((*a).x + (*a).radius, (*a).y + (*a).radius,
              (*b).x - (*b).radius, (*b).y - (*b).radius,
              (*b).x + (*b).radius, (*b).y + (*b).radius);

    if (TL || TR || BL || BR) {

      int alen = 0xffff;
      int blen = 0xffff;
      int ak = -1;       // nearest to b's center a's edge index
      int bk = -1;

      for (int k = 0; k < ASTEROID_EDGES_NUM; ++k) {
        int dx = (*b).x - ((*a).x + (*a).edges[(k << 1) + 0]);
        int dy = (*b).y - ((*a).y + (*a).edges[(k << 1) + 1]);
        int len = dx * dx + dy * dy;
        if (len < alen) {
          alen = len;
          ak = k;
        }

        dx = (*a).x - ((*b).x + (*b).edges[(k << 1) + 0]);
        dy = (*a).y - ((*b).y + (*b).edges[(k << 1) + 1]);
        len = dx * dx + dy * dy;
        if (len < blen) {
          blen = len;
          bk = k;
        }
      }

      int ax1 = (*a).x + (*a).edges[(ak << 1) + 0];
      int ay1 = (*a).y + (*a).edges[(ak << 1) + 1];

      int akm = ak > 0 ? ak - 1 : ASTEROID_EDGES_NUM - 1;
      int ax2 = (*a).x + (*a).edges[(akm << 1) + 0];
      int ay2 = (*a).y + (*a).edges[(akm << 1) + 1];

      int akp = ak < ASTEROID_EDGES_NUM - 1 ? ak + 1 : 0;
      int ax3 = (*a).x + (*a).edges[(akp << 1) + 0];
      int ay3 = (*a).y + (*a).edges[(akp << 1) + 1];

      int bx1 = (*b).x + (*b).edges[(bk << 1) + 0];
      int by1 = (*b).y + (*b).edges[(bk << 1) + 1];

      int bkm = bk > 0 ? bk - 1 : ASTEROID_EDGES_NUM - 1;
      int bx2 = (*b).x + (*b).edges[(bkm << 1) + 0];
      int by2 = (*b).y + (*b).edges[(bkm << 1) + 1];

      int s1 = get_line_intersection(ax1, ay1, ax2, ay2, bx1, by1, bx2, by2, NULL, NULL);
      int s2 = get_line_intersection(ax1, ay1, ax3, ay3, bx1, by1, bx2, by2, NULL, NULL);

      if (s1 || s2) {
        const float tms = (*a).mass + (*b).mass;
        const float avx = (*a).velx;
        const float avy = (*a).vely;
        const float bvx = (*b).velx;
        const float bvy = (*b).vely;

        (*a).velx = (avx * ((*a).mass - (*b).mass)
                   + bvx * ((*b).mass + (*b).mass)) / (tms);

        (*a).vely = (avy * ((*a).mass - (*b).mass)
                   + bvy * ((*b).mass + (*b).mass)) / (tms);

        (*b).velx = (avx * ((*a).mass + (*a).mass)
                   + bvx * ((*b).mass - (*a).mass)) / (tms);

        (*b).vely = (avy * ((*a).mass + (*a).mass)
                   + bvy * ((*b).mass - (*a).mass)) / (tms);

        return 1;
      }

    }

  }

  return 0;
}


int out_of_border(int i, int window_width, int window_height) {
  asteroid_t *a = &asteroids[i];
  return
    (*a).x - (*a).radius > window_width  ||
    (*a).x + (*a).radius < 0             ||
    (*a).y - (*a).radius > window_height ||
    (*a).y + (*a).radius < 0;
}


void proc_border_collision(int i, int window_width, int window_height) {
  asteroid_t *a = &asteroids[i];
  if      ((*a).velx > 0 && (*a).x + (*a).radius >= window_width ) (*a).velx *= -1;
  else if ((*a).velx < 0 && (*a).x - (*a).radius <= 0            ) (*a).velx *= -1;
  if      ((*a).vely > 0 && (*a).y + (*a).radius >= window_height) (*a).vely *= -1;
  else if ((*a).vely < 0 && (*a).y - (*a).radius <= 0            ) (*a).vely *= -1;
}





#endif // ASTEROIDS_H
