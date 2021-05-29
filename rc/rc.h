#ifndef RC_H
#define RC_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "bmp.h"
#include "bmp_font.h"


#define max(x, y) ( x > y ? x : y )
#define min(y, x) ( x < y ? x : y )


///////////////////////////////////////
// RC_Display
///////////////////////////////////////


typedef struct RC_Display {
  uint16_t width;
  uint16_t height;
  uint32_t *array;
} RC_display_t;


void
RC_init_display ( RC_display_t *d, uint16_t width, uint16_t height ) {
  ( *d ).width = width;
  ( *d ).height = height;
  ( *d ).array = calloc ( width * height, sizeof ( uint32_t ) );
}


void
RC_kill_display ( RC_display_t *d ) {
  free ( ( *d ).array );
}


void
RC_clear_display ( RC_display_t *d ) {
  for ( int i = 0; i < ( *d ).width * ( *d ).height; ++i )
    ( *d ).array [ i ] = 0;
}


///////////////////////////////////////
// RC_Map
///////////////////////////////////////


typedef struct RC_Map {
  uint16_t width;
  uint16_t height;
  uint8_t *array;
} RC_map_t;


void
RC_fill_map_1 ( RC_map_t *m ) {
  #define MAP_1_WIDTH 10
  #define MAP_1_HEIGHT 10
  const uint8_t map_1 [ MAP_1_WIDTH * MAP_1_HEIGHT ] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  };
  if ( m->array != NULL ) free ( m->array );
  ( *m ).width = MAP_1_WIDTH;
  ( *m ).height = MAP_1_HEIGHT;
  ( *m ).array = malloc ( ( *m ).width * ( *m ).height * sizeof ( uint8_t ) );
  for ( int i = 0; i < ( *m ).width * ( *m ).height; ++i )
    ( *m ).array [ i ] = map_1 [ i ];
}


void
RC_kill_map ( RC_map_t *m ) {
  free ( ( *m ).array );
}


bool
RC_map_solid_at ( const RC_map_t *m, int x, int y ) {
  if ( 0 <= x && x < ( *m ).width && 0 <= y && y < ( *m ).height )
    return ( *m ).array [ ( *m ).width * y + x ] == 1;
  else
    return true;
}


int
RC_map_wall_texture_at ( const RC_map_t *m, int x, int y ) {
  if ( 0 <= x && x < ( *m ).width && 0 <= y && y < ( *m ).height )
    return ( *m ).array [ ( *m ).width * y + x ];
  else
    return 0;
}


int
RC_map_floor_texture_at ( const RC_map_t *m, int x, int y ) {
  return 2;
  if ( 0 <= x && x < ( *m ).width && 0 <= y && y < ( *m ).height )
    return ( *m ).array [ ( *m ).width * y + x ];
  else
    return 0;
}


int
RC_map_ceiling_texture_at ( const RC_map_t *m, int x, int y ) {
  return 3;
  if ( 0 <= x && x < ( *m ).width && 0 <= y && y < ( *m ).height )
    return ( *m ).array [ ( *m ).width * y + x ];
  else
    return 0;
}


///////////////////////////////////////
// RC_Camera
///////////////////////////////////////


typedef struct RC_Camera {
  double rotation;
  double directionx;
  double directiony;
  double planex;
  double planey;
  double x;
  double y;
} RC_camera_t;


void
RC_init_camera ( RC_camera_t *camera ) {
  camera->rotation = 0.0;
  camera->directionx = -1.0;
  camera->directiony = +0.0;
  camera->planex = 0.00;
  camera->planey = 0.66;
  camera->x = 0.0;
  camera->y = 0.0;
}


void
RC_rotate_camera ( RC_camera_t *camera, double delta_angle ) {
  camera->rotation += delta_angle;
  const double cosa = cos ( delta_angle );
  const double sina = sin ( delta_angle );
  const double directionx_o = camera->directionx;
  const double directiony_o = camera->directiony;
  camera->directionx = directionx_o * cosa - directiony_o * sina;
  camera->directiony = directionx_o * sina + directiony_o * cosa;
  const double planex_o = camera->planex;
  const double planey_o = camera->planey;
  camera->planex = planex_o * cosa - planey_o * sina;
  camera->planey = planex_o * sina + planey_o * cosa;
}


///////////////////////////////////////
// Textures
///////////////////////////////////////


int
RC_load_textures ( const char ( *paths ) [ 128 ], const int length, BMP_obj_t **textures, int *err ) {
  ( *textures ) = malloc ( length * sizeof ( BMP_obj_t ) );
  for ( int i = 0; i < length; ++i ) {
    if ( BMP_read ( & ( *textures ) [ i ], paths [ i ] ) < 0 ) {
      if ( err ) ( *err ) = i;
      return -1;
    }
    BMP_mirror ( & ( *textures ) [ i ] );
  }

  return 0;
}


void
RC_kill_textures ( const int length, BMP_obj_t *textures ) {
  for ( int i = 0; i < length; ++i ) BMP_kill ( &textures [ i ] );
  free ( textures );
}


///////////////////////////////////////
// Common
///////////////////////////////////////


void
RC_cast_walls (
  RC_display_t *display,
  const RC_map_t *map,
  const RC_camera_t *camera,
  const BMP_obj_t *textures )
{
  const int distance_max = 15;
  const double brightness = ( double ) distance_max + 1;

  for ( int px = 0; px < display->width; px += 1 ) {

    // Display x in camera space.
    const double cx = 2.0 * px / display->width - 1;
    const double ray_dir_x = camera->directionx + camera->planex * cx;
    const double ray_dir_y = camera->directiony + camera->planey * cx;

    // Length of ray from one x or y-side to next x or y-side.
    const double delta_x = fabs ( 1.0 / ray_dir_x );
    const double delta_y = fabs ( 1.0 / ray_dir_y );

    // Which box of the map we're in.
    int map_x = ( int ) camera->x;
    int map_y = ( int ) camera->y;

    // Length of ray from current position to next x or y-side.
    double side_dist_x;
    double side_dist_y;

    // What direction to step in x or y-direction (either +1 or -1).
    int step_x;
    int step_y;

    // Calculating step and initial side_dist.
    if ( ray_dir_x < 0 ) { step_x = -1; side_dist_x = -delta_x * ( ( double ) map_x - camera->x     ); }
    else                 { step_x = +1; side_dist_x = +delta_x * ( ( double ) map_x - camera->x + 1 ); }
    if ( ray_dir_y < 0 ) { step_y = -1; side_dist_y = -delta_y * ( ( double ) map_y - camera->y     ); }
    else                 { step_y = +1; side_dist_y = +delta_y * ( ( double ) map_y - camera->y + 1 ); }

    // Performing DDA.
    bool side; // Was a NS or a EW wall hit?
    bool hit = false; // Was there a wall hit?
    for ( int di = 0; !hit && di < distance_max; ++di ) {
      if ( side_dist_x < side_dist_y ) { side_dist_x += delta_x; map_x += step_x; side = false; }
      else                             { side_dist_y += delta_y; map_y += step_y; side = true;  }
      hit = RC_map_solid_at ( map, map_x, map_y );
    }

    if ( hit ) {

      // Distance of perpendicular ray ( Euclidean distance will give fisheye effect! )
      const double perp_wall_dist =
        side
          ? ( map_y - camera->y + 0.5 - step_y / 2.0 ) / ray_dir_y
          : ( map_x - camera->x + 0.5 - step_x / 2.0 ) / ray_dir_x;

      // Height of strip to draw on screen.
      const int strip_height = ( int ) ( display->height / perp_wall_dist );

      // Lowest and highest pixel to fill in current strip.
      const int strip_y0 = fmax ( 0,               ( display->height - strip_height ) / 2 );
      const int strip_y1 = fmin ( display->height, ( display->height + strip_height ) / 2 );

      // Texture
      const int texture_index = RC_map_wall_texture_at ( map, map_x, map_y );
      const BMP_obj_t *texture = &textures [ texture_index ];

      // How much to increase the texture coordinate per screen pixel?
      const double texture_step = ( double ) texture->height / strip_height;

      // Starting texture coordinate.
      double texture_pos = ( strip_y0 - display->height / 2.0 + strip_height / 2.0 ) * texture_step;

      // Where exactly the wall was hit?
      double texture_x =
        side
          ? ( camera->x + perp_wall_dist * ray_dir_x )
          : ( camera->y + perp_wall_dist * ray_dir_y );
      texture_x -= ( int ) texture_x;

      // Texture x coordinate
      int tx = ( int ) ( texture->width * texture_x );
      if ( side ^ ray_dir_x > 0 ) tx = texture->width - tx - 1;

      // Fade out by distance
      double fade = fmin ( 1.0, fmax ( 0.1, 1 - ( perp_wall_dist / brightness ) ) );
      if ( side ) fade /= 2.0;

      for ( int py = strip_y0; py < strip_y1; py += 1 ) {

        // Cast the texture coordinate to integer, and mask with (texture_height - 1) in case of overflow.
        const int ty = ( int ) texture_pos & ( texture->height - 1 );
        texture_pos += texture_step;

        // Apply pixels to the display.
        const uint8_t b = ( uint8_t ) ( texture->bgr [ ( texture->width * ty + tx ) * 3 + 0 ] * fade );
        const uint8_t g = ( uint8_t ) ( texture->bgr [ ( texture->width * ty + tx ) * 3 + 1 ] * fade );
        const uint8_t r = ( uint8_t ) ( texture->bgr [ ( texture->width * ty + tx ) * 3 + 2 ] * fade );
        const uint32_t rgb = ( r << 0x10 ) | ( g << 0x08 ) | ( b << 0x00 );
        display->array [ display->width * py + px ] = rgb;
      }

      // minimap
      // display->array [ display->width * map_y + map_x ] = 0xff0000;
    }
  }
}


void
RC_cast_surfaces (
  RC_display_t *display,
  const RC_map_t *map,
  const RC_camera_t *camera,
  const BMP_obj_t *textures )
{
  const bool ignore_ceiling = false;
  const bool ignore_floor = false;
  const int floor_limit = 1; // blank part of floor/ceiling from screen center in pixels
  const double brightness = 10.0;

  for( int y = 0; y < display->height / 2 - floor_limit; ++y ) {

    // ray_dir for leftmost ray (x = 0) and rightmost ray (x = w)
    const double ray_dir_x0 = camera->directionx - camera->planex;
    const double ray_dir_y0 = camera->directiony - camera->planey;
    const double ray_dir_x1 = camera->directionx + camera->planex;
    const double ray_dir_y1 = camera->directiony + camera->planey;

    // Vertical position of the camera.
    const double pos_z = 0.5 * display->height;

    // Current y position compared to the center of the screen (the horizon).
    const double p = display->height / 2.0 - y;

    // Horizontal distance from the camera to the floor for the current row.
    // 0.5 is the z position exactly in the middle between floor and ceiling.
    const double row_dist = pos_z / ( p ? p : 1 );

    // calculate the real world step vector we have to add for each x (parallel to camera plane)
    // adding step by step avoids multiplications with a weight in the inner loop
    const double step_x = row_dist * ( ray_dir_x1 - ray_dir_x0 ) / display->width;
    const double step_y = row_dist * ( ray_dir_y1 - ray_dir_y0 ) / display->width;

    // Fade out by distance.
    const double fade = fmin ( 1.0, fmax ( 0.1, 1 - ( row_dist / brightness ) ) );

    // Real world coordinates of the leftmost column. This will be updated as we step to the right.
    double floor_x = camera->x + row_dist * ray_dir_x0;
    double floor_y = camera->y + row_dist * ray_dir_y0;

    for( int x = 0; x < display->width; ++x ) {

      // The cell coord is simply got from the integer parts of floor_x and floor_y.
      const int cell_x = ( int ) floor_x;
      const int cell_y = ( int ) floor_y;

      // Updating real world coordinates.
      floor_x += step_x;
      floor_y += step_y;

      if ( !ignore_ceiling ) {

        // Texture
        const int texture_index = RC_map_ceiling_texture_at ( map, cell_x, cell_y );
        const BMP_obj_t *texture = &textures [ texture_index ];

        // Gets the texture coordinate from the fractional part.
        const int tx = ( int ) ( texture->width  * ( floor_x - cell_x ) ) & ( texture->width  - 1 );
        const int ty = ( int ) ( texture->height * ( floor_y - cell_y ) ) & ( texture->height - 1 );

        // Apply pixels to the display.
        const uint8_t b = ( uint8_t ) ( texture->bgr [ ( texture->width * ty + tx ) * 3 + 0 ] * fade );
        const uint8_t g = ( uint8_t ) ( texture->bgr [ ( texture->width * ty + tx ) * 3 + 1 ] * fade );
        const uint8_t r = ( uint8_t ) ( texture->bgr [ ( texture->width * ty + tx ) * 3 + 2 ] * fade );
        const uint32_t rgb = ( r << 0x10 ) | ( g << 0x08 ) | ( b << 0x00 );
        display->array [ display->width * y + x ] = rgb;
      }

      if ( !ignore_floor ) {

        // Texture
        const int texture_index = RC_map_floor_texture_at ( map, cell_x, cell_y );
        const BMP_obj_t *texture = &textures [ texture_index ];

        // Gets the texture coordinate from the fractional part.
        const int tx = ( int ) ( texture->width  * ( floor_x - cell_x ) ) & ( texture->width  - 1 );
        const int ty = ( int ) ( texture->height * ( floor_y - cell_y ) ) & ( texture->height - 1 );

        // Apply pixels to the display.
        const uint8_t b = ( uint8_t ) ( texture->bgr [ ( texture->width * ty + tx ) * 3 + 0 ] * fade );
        const uint8_t g = ( uint8_t ) ( texture->bgr [ ( texture->width * ty + tx ) * 3 + 1 ] * fade );
        const uint8_t r = ( uint8_t ) ( texture->bgr [ ( texture->width * ty + tx ) * 3 + 2 ] * fade );
        const uint32_t rgb = ( r << 0x10 ) | ( g << 0x08 ) | ( b << 0x00 );
        display->array [ display->width * ( display->height - y - 1 ) + x ] = rgb;
      }
    }
  }
}


void
RC_cast_sprite (
  RC_display_t *display,
  const RC_camera_t *camera,
  const BMP_obj_t *texture,
  double sx,
  double sy )
{
  // _________     __________   __________     [ camera.planex   camera.dirx ] ( -1 )
  // transform = ( sprite.pos - camera.pos ) x |                             |
  //                                           [ camera.planey   camera.diry ]
  const double det = 1.0 / ( camera->planex * camera->directiony - camera->directionx * camera->planey );
  const double rx = sx - camera->x;
  const double ry = sy - camera->y;
  const double transformx = det * ( rx * camera->directiony - ry * camera->directionx );
  const double transformy = det * ( ry * camera->planex     - rx * camera->planey );

  const int spritex = ( int ) ( display->width / 2 * ( 1 + transformx / transformy ) );
  const int spriteh = abs ( ( int ) ( display->height / transformy ) );
  const int spritew = spriteh;
  const int spritey0 = max ( display->height / 2 - spriteh / 2, 0 );
  const int spritey1 = min ( display->height / 2 + spriteh / 2, display->height - 1 );
  const int spritex0 = max ( spritex - spritew / 2, 0 );
  const int spritex1 = min ( spritex + spritew / 2, display->width - 1 );

  for ( int x = spritex0; x < spritex1; ++x ) {

    if ( !( transformy > 0 )                // out of near plane
    ||   !( 0 < x && x < display->width ) // out of screen bounds
    //||   !( transformy < zbuff[x] )       // test z-buffer
    )
    {
      continue;
    }

    const int texx = ( int ) ( ( x - ( spritex - spritew / 2 ) ) * texture->width / ( double ) spritew );

    for ( int y = spritey0; y < spritey1; ++y ) {

      const int d = y - ( display->height - spriteh ) / 2;
      const int texy = d * texture->height / spriteh;

      const int i = ( texture->width * texy + texx ) * 3;
      const uint32_t color
        = ( texture->bgr [ i + 2 ] << 0x10 )
        | ( texture->bgr [ i + 1 ] << 0x08 )
        | ( texture->bgr [ i + 0 ] << 0x00 ) ;
      if ( color == 0x00ffff ) continue; // alpha

      display->array [ display->width * y + x ] = color;
    }
  }
}


bool
RC_test_collision ( const RC_map_t *map, const double r, double nx, double ny, double *outx, double *outy ) {
  // Returns true if outx/outy are updated.

  const int x = ( int ) nx;
  const int y = ( int ) ny;

  if ( RC_map_solid_at ( map, x, y ) ) return false;

  const bool bt = RC_map_solid_at ( map, x    , y - 1 );
  const bool bb = RC_map_solid_at ( map, x    , y + 1 );
  const bool bl = RC_map_solid_at ( map, x - 1, y     );
  const bool br = RC_map_solid_at ( map, x + 1, y     );

  if ( bt && 0 - y + ny < r ) ny = 0 + y + r;
  if ( bb && 1 + y - ny < r ) ny = 1 + y - r;
  if ( bl && 0 - x + nx < r ) nx = 0 + x + r;
  if ( br && 1 + x - nx < r ) nx = 1 + x - r;

  const double r2 = r * r;

  if ( !( bt && bl ) && RC_map_solid_at ( map, x - 1, y - 1 ) ) {
    const double dx = nx - ( x + 0 );
    const double dy = ny - ( y + 0 );
    const double qx = dx * dx;
    const double qy = dy * dy;
    if ( qx + qy < r2 )
      ( qx > qy )
        ? ( nx = x + 0 + r )
        : ( ny = y + 0 + r );
  }

  if ( !( bt && br ) && RC_map_solid_at ( map, x + 1, y - 1 ) ) {
    const double dx = nx - ( x + 1 );
    const double dy = ny - ( y + 0 );
    const double qx = dx * dx;
    const double qy = dy * dy;
    if ( qx + qy < r2 )
      ( qx > qy )
        ? ( nx = x + 1 - r )
        : ( ny = y + 0 + r );
  }

  if ( !( bb && bb ) && RC_map_solid_at ( map, x - 1, y + 1 ) ) {
    const double dx = nx - ( x + 0 );
    const double dy = ny - ( y + 1 );
    const double qx = dx * dx;
    const double qy = dy * dy;
    if ( qx + qy < r2 )
      (qx > qy)
        ? ( nx = x + 0 + r )
        : ( ny = y + 1 - r );
  }

  if ( !( bb && br ) && RC_map_solid_at ( map, x + 1 , y + 1 ) ) {
    const double dx = nx - ( x + 1 );
    const double dy = ny - ( y + 1 );
    const double qx = dx * dx;
    const double qy = dy * dy;
    if ( qx + qy < r2 )
      (qx > qy)
        ? ( nx = x + 1 - r )
        : ( ny = y + 1 - r );
  }

  ( *outx ) = nx;
  ( *outy ) = ny;

  return true;
}


void
RC_draw_text ( RC_display_t *d, const BMP_font_t *f, const unsigned char *text, uint16_t x, uint16_t y ) {
  for ( int ti = 0; text [ ti ] != '\0'; ++ti ) {
    const unsigned char *character = ( *f ).table [ text [ ti ] ];
    for ( int cy = 0; cy < ( *f ).ch; ++cy )
      for ( int cx = 0; cx < ( *f ).cw; ++cx )
    {
      if ( character [ ( *f ).cw * cy + cx ] ) {
        if ( x + cx < 0 || x + cx >= ( *d ).width || y + cy < 0 ) break;
        if ( y + cy >= ( *d ).height ) return;
        ( *d ).array [ ( *d ).width * ( y + cy ) + ( x + cx ) ] = 0xffffff;
      }
    }
    x += ( *f ).cw;
  }
}


#endif // RC_H
