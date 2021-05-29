#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include "SDL.h"
#include "rc.h"


int
main ( int argc, char **args ) {
  SDL_SetHint ( SDL_HINT_RENDER_DRIVER, "opengl" );

  if ( SDL_Init ( SDL_INIT_VIDEO ) < 0 ) {
    fprintf (stderr, "SDL_Init: %s\n", SDL_GetError () );
    return 1;
  }

  #define WINDOW_WIDTH 400
  #define WINDOW_HEIGHT 400
  SDL_WindowFlags window_flags = 0;
  SDL_Window *window = SDL_CreateWindow (
    "rc",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    WINDOW_WIDTH,
    WINDOW_HEIGHT,
    window_flags
  );
  if ( window == NULL ) {
    fprintf ( stderr, "SDL_CreateWindow: %s\n", SDL_GetError () );
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer (
    window,
    -1,
    SDL_RENDERER_ACCELERATED
  );
  if ( renderer == NULL ) {
    fprintf ( stderr, "SDL_CreateRenderer: %s\n", SDL_GetError () );
    return 1;
  }

  #define CANVAS_WIDTH 128
  #define CANVAS_HEIGHT 128
  SDL_Texture *canvas = SDL_CreateTexture (
    renderer,
    SDL_PIXELFORMAT_RGB24,
    SDL_TEXTUREACCESS_STREAMING,
    CANVAS_WIDTH,
    CANVAS_HEIGHT
  );
  if ( canvas == NULL ) {
    fprintf ( stderr, "SDL_CreateTexture: %s\n", SDL_GetError () );
    return 1;
  }


  // Setup

  SDL_SetRelativeMouseMode ( SDL_TRUE );

  RC_display_t display;
  RC_init_display ( &display, CANVAS_WIDTH, CANVAS_HEIGHT );

  RC_map_t map;
  RC_fill_map_1 ( &map );

  RC_camera_t camera;
  RC_init_camera ( &camera );
  camera.x = 5;
  camera.y = 5;

  BMP_font_t font;
  if ( BMP_font_init ( &font, "fixedsys8x12.bmp" ) < 0 ) {
    fprintf ( stderr, "BMP_font_init: cannot found fixedsys8x12.bmp\n" );
    return 1;
  }

  const double collision_radius = 0.5;

  // Main Loop

  bool is_running = true;
  bool is_fullscreen = false;
  bool is_mouse_locked = true;
  bool keys [ 256 ];
  for ( int i = 0; i < 256; ++i ) keys [ i ] = false;

  uint32_t previous_time = SDL_GetTicks ();

  const uint32_t fps_delay = 1000;
  uint32_t fps_timer = fps_delay;
  uint32_t fps_value;


  while ( is_running ) {
    const uint32_t current_time = SDL_GetTicks ();
    const uint32_t elapsed = current_time - previous_time;
    previous_time = current_time;

    int mouse_delta_x = 0;

    // Events

    SDL_Event event;
    while ( SDL_PollEvent ( &event ) ) {

      // Window

      if ( event.type == SDL_QUIT ) {
        is_running = false;
      }

      // Mouse

      else if ( event.type == SDL_MOUSEMOTION ) {
        mouse_delta_x = event.motion.xrel;
      }

      else if ( event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT ) {
        if ( is_mouse_locked == false ) {
          is_mouse_locked = true;
          SDL_SetRelativeMouseMode ( SDL_TRUE );
        }
      }

      // Keyboard

      else if ( event.type == SDL_KEYDOWN ) {

        keys [ ( uint8_t ) event.key.keysym.sym ] = true;

        if ( event.key.keysym.sym == SDLK_ESCAPE ) {
          if ( is_mouse_locked == true ) {
            is_mouse_locked = false;
            SDL_SetRelativeMouseMode ( SDL_FALSE );
          }
        }
        else if ( event.key.keysym.sym == 'f' ) {
          is_fullscreen = !is_fullscreen;
          SDL_SetWindowFullscreen (
            window,
            is_fullscreen
              ? window_flags | SDL_WINDOW_FULLSCREEN_DESKTOP
              : window_flags
          );
        }

      }

      else if ( event.type == SDL_KEYUP ) {

        keys [ ( uint8_t ) event.key.keysym.sym ] = false;

      }

    }

    // Mouse

    // turn left / turn right
    if ( mouse_delta_x != 0 && is_mouse_locked ) {
      RC_rotate_camera ( &camera, mouse_delta_x * -0.01 );
    }

    // Keyboard

    // turn left / turn right
    if        ( keys [ 'q' ] ) {
      RC_rotate_camera ( &camera, +0.01 );
    } else if ( keys [ 'e' ] ) {
      RC_rotate_camera ( &camera, -0.01 );
    }
    // move forward / move backward
    if        ( keys [ 'w' ] ) {
      const double new_camera_x = camera.x + camera.directionx * +0.02;
      const double new_camera_y = camera.y + camera.directiony * +0.02;
      RC_test_collision ( &map, collision_radius, new_camera_x, new_camera_y, &camera.x, &camera.y );
    } else if ( keys [ 's' ] ) {
      const double new_camera_x = camera.x + camera.directionx * -0.02;
      const double new_camera_y = camera.y + camera.directiony * -0.02;
      RC_test_collision ( &map, collision_radius, new_camera_x, new_camera_y, &camera.x, &camera.y );
    }
    // strafe left / strafe right
    if        ( keys [ 'a' ] ) {
      const double new_camera_x = camera.x + camera.planex * -0.02;
      const double new_camera_y = camera.y + camera.planey * -0.02;
      RC_test_collision ( &map, collision_radius, new_camera_x, new_camera_y, &camera.x, &camera.y );
    } else if ( keys [ 'd' ] ) {
      const double new_camera_x = camera.x + camera.planex * +0.02;
      const double new_camera_y = camera.y + camera.planey * +0.02;
      RC_test_collision ( &map, collision_radius, new_camera_x, new_camera_y, &camera.x, &camera.y );
    }

    // Render

    RC_clear_display ( &display );

    for ( int y = 0; y < map.height; ++y ) {
      for ( int x = 0; x < map.width; ++x ) {
        if ( RC_map_solid_at ( &map, x, y ) ) {
          display.array [ display.width * y + x ] = 100;
        }
      }
    }

    RC_cast_walls ( &display, &map, &camera );

    fps_timer += elapsed;
    if ( fps_timer >= fps_delay ) {
      fps_value = elapsed == 0 ? 0 : 1000 / elapsed;
      fps_timer = 0;
    }
    char fps_value_str [ 100 ];
    sprintf( fps_value_str, "fps: %d", fps_value );
    RC_draw_text ( &display, &font, fps_value_str, 0, CANVAS_HEIGHT - 12 );

    // Flush

    uint8_t *pixels;
    int pitch;
    SDL_LockTexture ( canvas, NULL, ( void** ) &pixels, &pitch );

    for ( int i = 0; i < display.width * display.height; ++i ) {
      const uint8_t rgb = display.array [ i ];
      *( pixels + 0 ) = rgb;
      *( pixels + 1 ) = 0;
      *( pixels + 2 ) = 64;
      pixels += 3;
    }

    SDL_UnlockTexture ( canvas );
    SDL_RenderCopy ( renderer, canvas, NULL, NULL );
    SDL_RenderPresent ( renderer );

    // debug
    if ( true & false ) {
      SDL_Delay ( 2000 );
      break;
    }
  }

  // Destroy

  BMP_font_kill ( &font );

  RC_kill_map ( &map );
  RC_kill_display ( &display );

  SDL_DestroyRenderer ( renderer );
  SDL_DestroyWindow ( window );
  SDL_Quit ();
  return 0;
}
