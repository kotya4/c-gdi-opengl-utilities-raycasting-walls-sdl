#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "SDL.h"
#include "rc.h"
#include "map.h"
#include "utils.h"
#include "bmp_d.h"


typedef struct Game {

  SDL_WindowFlags window_flags;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *canvas;

  RC_display_t display;
  RC_map_t map;
  RC_camera_t camera;
  BMP_font_t font;

  double collision_radius;

  double kb_look_speed;
  double m_look_speed;
  double move_speed;

  bool keys [ 256 ];

  uint32_t previous_time;
  uint32_t current_time;
  uint32_t elapsed;

  uint32_t fps_delay;
  uint32_t fps_timer;
  uint32_t fps_value;

  bool is_running;
  bool is_fullscreen;
  bool is_mouse_locked;

  int mouse_delta_x;

  BMP_obj_t *textures;
  int textures_length;

  double spritex;
  double spritey;


  texture_t *itexs;
  BMP_indexed_t *ibmps;
  int ibmps_index;
  size_t ibmps_length;

  int map_index;
  bool map_fromstart;

} game_t;


int init ( game_t * );
int setup ( game_t * );
int listen ( game_t * );
int process ( game_t * );
int render ( game_t * );
int kill ( game_t * );
int generate_level ( game_t * );


int
main ( int argc, char **args ) {
  #define WINDOW_WIDTH 640
  #define WINDOW_HEIGHT 480

  #define CANVAS_WIDTH 256
  #define CANVAS_HEIGHT 256

  game_t game;

  if ( init ( &game ) < 0 ) return -1;

  setup ( &game );

  while ( game.is_running ) {
    game.current_time = SDL_GetTicks ();
    game.elapsed = game.current_time - game.previous_time;
    game.previous_time = game.current_time;

    game.mouse_delta_x = 0;

    listen ( &game );

    process ( &game );

    render ( &game );

    // SDL_Delay(1000); break;
  }

  kill ( &game );

  return 0;
}



int
init ( game_t *g ) {
  int status;




  g->ibmps_length = 0;
  g->ibmps_index = 0;
  g->ibmps = NULL;
  status = BMP_d ( &g->ibmps, &g->ibmps_length );
  if ( status < 0 ) {
    printf ( "BMP_d: status %d\n", status );
    return -1;
  }
  g->itexs = malloc ( g->ibmps_length * sizeof *g->itexs );
  for ( int i = 0; i < g->ibmps_length; ++i ) {
    g->itexs[ i ].ibmp = &g->ibmps[ i ];
    for ( int k = 0; k < g->ibmps[ i ].palette.length; ++k ) {
      if ( g->ibmps[ i ].palette.colors[ k ].r == 0 &&
      g->ibmps[ i ].palette.colors[ k ].g == 0 &&
      g->ibmps[ i ].palette.colors[ k ].b == 0 )
      {
        g->itexs[ i ].transparent = true;
        break;
      }
    }
  }




  SDL_SetHint ( SDL_HINT_RENDER_DRIVER, "opengl" );

  if ( SDL_Init ( SDL_INIT_VIDEO ) < 0 ) {
    fprintf (stderr, "SDL_Init: %s\n", SDL_GetError () );
    return -1;
  }

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
    return -1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer (
    window,
    -1,
    SDL_RENDERER_ACCELERATED
  );
  if ( renderer == NULL ) {
    fprintf ( stderr, "SDL_CreateRenderer: %s\n", SDL_GetError () );
    return -1;
  }

  SDL_Texture *canvas = SDL_CreateTexture (
    renderer,
    SDL_PIXELFORMAT_RGB24,
    SDL_TEXTUREACCESS_STREAMING,
    CANVAS_WIDTH,
    CANVAS_HEIGHT
  );
  if ( canvas == NULL ) {
    fprintf ( stderr, "SDL_CreateTexture: %s\n", SDL_GetError () );
    return -1;
  }

  g->window_flags = window_flags;
  g->window = window;
  g->renderer = renderer;
  g->canvas = canvas;

  RC_init_display ( &g->display, CANVAS_WIDTH, CANVAS_HEIGHT );
  RC_init_camera ( &g->camera );
  const char font_filepath [] = "fixedsys8x12.bmp";
  if ( BMP_font_init ( &g->font, font_filepath ) < 0 ) {
    fprintf ( stderr, "BMP_font_init: cannot open %s\n", font_filepath );
    return -1;
  }

  g->is_fullscreen = false;






  #define TEXTURES_LENGTH 6
  const char textures_paths [ TEXTURES_LENGTH ] [ 128 ] = {
    "tex/empty.bmp",
    "tex/ancientx64.bmp",
    "tex/ancientfloorx64.bmp",
    "tex/ancientceilingx64.bmp",
    "tex/ancientstatuex64.bmp",
    "tex/voided.bmp"
  };
  BMP_obj_t *textures;
  int err_textures_index;
  status =
    RC_load_textures
      ( textures_paths
      , TEXTURES_LENGTH
      , &textures
      , &err_textures_index
      ) ;
  if ( status < 0 ) {
    fprintf
      ( stderr
      , "RC_load_textures: cannot open %s\n"
      , textures_paths [ err_textures_index ]
      );
    return -1;
  }
  g->textures_length = TEXTURES_LENGTH;
  g->textures = textures;






  g->spritex = 3;
  g->spritey = 3;
  g->map_index = 0;
  g->map_fromstart = true;

  double values[ 8 ] = { 0,-1,0,0,0.66,-1,-1,0 };
  #define FILEPATH "save.txt"
  FILE *f = fopen ( FILEPATH, "r" );
  if ( !f ) {
    printf ( "fopen: cannot open file %s\n", FILEPATH );
  } else {
    g->map_fromstart = false;
    for ( int i = 0; i < 8; ++i ) {
      #define STR_LEN 64
      char str[ STR_LEN ];
      if ( fgets ( str, STR_LEN, f ) == NULL ) break;
      sscanf ( str, "%lf", &values[ i ] );
    }
    fclose ( f );
  }
  g->camera.rotation = values[ 0 ];
  g->camera.directionx = values[ 1 ];
  g->camera.directiony = values[ 2 ];
  g->camera.planex = values[ 3 ];
  g->camera.planey = values[ 4 ];
  g->camera.x = values[ 5 ];
  g->camera.y = values[ 6 ];
  g->map_index = values[ 7 ];

  return 0;
}




int
generate_level ( game_t *g ) {
  srand ( g->map_index );
  // RC_fill_map_1 ( &g->map );
  // g->map.array[ g->map.width * g->map.height / 2 + g->map.width / 2 ] = 5;

  // MAP_map_t map;
  MAP_gen ( &g->map, g->itexs, g->ibmps_length );
  // g->map.array = malloc ( map.length * sizeof *g->map.array );
  // g->map.width = map.width;
  // g->map.height = map.height;
  // for ( int i = 0; i < map.length; ++i ) {
    // g->map.array[ i ] = map.array[ i ] == 1;
  // }

  if ( g->map_fromstart ) {
    g->map_fromstart = false;
    // place player
    g->camera.x = 1;
    g->camera.y = 1;
    const size_t offseti = rand();
    for ( int i = 0; i < g->map.length; ++i ) {
      if ( MAP_isempty ( g->map.array[ i + offseti ] ) ) {
        g->camera.x = (i + offseti) / g->map.width + 0.5;
        g->camera.y = (i + offseti) % g->map.width + 0.5;
        break;
      }
    }
  }

}




int
setup ( game_t *g ) {

  generate_level ( g );

  g->collision_radius = 0.25;

  g->kb_look_speed = 0.003;
  g->m_look_speed  = 0.007;
  g->move_speed    = 0.002;

  for ( int i = 0; i < 256; ++i ) g->keys [ i ] = false;

  g->previous_time = SDL_GetTicks ();
  g->current_time = 0;
  g->elapsed = 0;

  g->fps_delay = 1000;
  g->fps_timer = g->fps_delay;
  g->fps_value = 0;

  g->is_running = true;
  g->is_mouse_locked = true;
  g->mouse_delta_x = 0;

  SDL_SetRelativeMouseMode ( SDL_TRUE );

  return 0;
}




int
listen ( game_t *g ) {

  SDL_Event event;
  while ( SDL_PollEvent ( &event ) ) {

    // Window

    if ( event.type == SDL_QUIT ) {
      g->is_running = false;
    }

    // Mouse

    else if ( event.type == SDL_MOUSEMOTION ) {
      g->mouse_delta_x = event.motion.xrel;
    }

    else if ( event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT ) {
      if ( g->is_mouse_locked == false ) {
        g->is_mouse_locked = true;
        SDL_SetRelativeMouseMode ( SDL_TRUE );
      }
    }

    // Keyboard

    else if ( event.type == SDL_KEYDOWN ) {

      g->keys [ ( uint8_t ) event.key.keysym.sym ] = true;

      if ( event.key.keysym.sym == SDLK_ESCAPE ) {
        // if ( g->is_mouse_locked == true ) {
          // g->is_mouse_locked = false;
          // SDL_SetRelativeMouseMode ( SDL_FALSE );
        // }
        g->is_running = false;

        double values[ 8 ] = {
          g->camera.rotation,
          g->camera.directionx,
          g->camera.directiony,
          g->camera.planex,
          g->camera.planey,
          g->camera.x,
          g->camera.y,
          g->map_index
        };
        #define FILEPATH "save.txt"
        FILE *f = fopen ( FILEPATH, "w" );
        if ( !f ) {
          printf ( "fopen: cannot write file %s\n", FILEPATH );
        } else {
          for ( int i = 0; i < 8; ++i ) {
            fprintf ( f, "%f\n", values[ i ] );
          }
          fclose ( f );
        }



      }

      else if ( event.key.keysym.sym == 'f' ) {
        g->is_fullscreen = !g->is_fullscreen;
        SDL_SetWindowFullscreen (
          g->window,
          g->is_fullscreen
            ? g->window_flags | SDL_WINDOW_FULLSCREEN_DESKTOP
            : g->window_flags
        );
      }

      else if ( event.key.keysym.sym == '1' ) {
        g->spritex = g->camera.x;
        g->spritey = g->camera.y;
      }

      else if ( event.key.keysym.sym == '2' ) {
        g->ibmps_index = ( g->ibmps_index + 1 ) % g->ibmps_length;
      }

      else if ( event.key.keysym.sym == '3' ) {
        g->ibmps_index--;
        if ( g->ibmps_index < 0 ) g->ibmps_index = g->ibmps_length - 1;
      }

      else if ( event.key.keysym.sym == '4' ) {
        g->map_index++;
        g->map_fromstart = true;
        generate_level ( g );
      }

      else if ( event.key.keysym.sym == '5' ) {
        g->map_index--;
        g->map_fromstart = true;
        generate_level ( g );
      }
    }

    else if ( event.type == SDL_KEYUP ) {
      g->keys [ ( uint8_t ) event.key.keysym.sym ] = false;
    }

  }

  return 0;
}





int
process ( game_t *g ) {

  // Mouse

  // turn left / turn right
  if ( g->mouse_delta_x != 0 && g->is_mouse_locked ) {
    RC_rotate_camera ( &g->camera, -g->m_look_speed * g->mouse_delta_x );
  }

  // Keyboard

  // turn left / turn right
  if ( g->keys [ 'q' ] ) {
    RC_rotate_camera ( &g->camera, +g->kb_look_speed * g->elapsed );
  } else if ( g->keys [ 'e' ] ) {
    RC_rotate_camera ( &g->camera, -g->kb_look_speed * g->elapsed );
  }
  // move forward / move backward
  if ( g->keys [ 'w' ] ) {
    const double new_camera_x = g->camera.x + g->camera.directionx * +g->move_speed * g->elapsed;
    const double new_camera_y = g->camera.y + g->camera.directiony * +g->move_speed * g->elapsed;
    RC_test_collision
      ( &g->map
      , g->collision_radius
      , new_camera_x
      , new_camera_y
      , &g->camera.x
      , &g->camera.y
      );
  } else if ( g->keys [ 's' ] ) {
    const double new_camera_x = g->camera.x + g->camera.directionx * -g->move_speed * g->elapsed;
    const double new_camera_y = g->camera.y + g->camera.directiony * -g->move_speed * g->elapsed;
    RC_test_collision
      ( &g->map
      , g->collision_radius
      , new_camera_x
      , new_camera_y
      , &g->camera.x
      , &g->camera.y
      );
  }
  // strafe left / strafe right
  if ( g->keys [ 'a' ] ) {
    const double new_camera_x = g->camera.x + g->camera.planex * -g->move_speed * g->elapsed;
    const double new_camera_y = g->camera.y + g->camera.planey * -g->move_speed * g->elapsed;
    RC_test_collision
      ( &g->map
      , g->collision_radius
      , new_camera_x
      , new_camera_y
      , &g->camera.x
      , &g->camera.y
      );
  } else if ( g->keys [ 'd' ] ) {
    const double new_camera_x = g->camera.x + g->camera.planex * +g->move_speed * g->elapsed;
    const double new_camera_y = g->camera.y + g->camera.planey * +g->move_speed * g->elapsed;
    RC_test_collision
      ( &g->map
      , g->collision_radius
      , new_camera_x
      , new_camera_y
      , &g->camera.x
      , &g->camera.y
      );
  }

  return 0;
}





int
render ( game_t *g ) {

  RC_clear_display ( &g->display );

  RC_cast_surfaces ( &g->display, &g->map, &g->camera, g->itexs );

  RC_cast_walls ( &g->display, &g->map, &g->camera, g->itexs );

  double mindepth = g->display.depthbuffer[ 0 ];
  double maxdepth = g->display.depthbuffer[ 0 ];
  for ( int i = 1; i < g->display.length; ++i ) {
    mindepth = min ( g->display.depthbuffer[ i ], mindepth );
    maxdepth = max ( g->display.depthbuffer[ i ], maxdepth );
  }
  // char minmax_s [ 100 ];
  // sprintf ( minmax_s, "minmax_s: %f %f", mindepth, maxdepth );
  // RC_draw_text ( &g->display, &g->font, minmax_s, 0, CANVAS_HEIGHT - 12 * 2 );




  RC_cast_sprite ( &g->display, &g->camera, &g->textures[ 4 ], g->spritex, g->spritey );


  // minimap
  for ( int y = 0; y < g->map.height; ++y ) {
    for ( int x = 0; x < g->map.width; ++x ) {
      if ( RC_map_solid_at ( &g->map, x, y ) ) {
        g->display.array [ g->display.width * y + x ] = 0x880000;
      }
    }
  }

  // fps
  g->fps_timer += g->elapsed;
  if ( g->fps_timer >= g->fps_delay ) {
    g->fps_value = g->elapsed == 0 ? 0 : 1000 / g->elapsed;
    g->fps_timer = 0;
  }
  char fps_value_str [ 10 ];
  sprintf ( fps_value_str, "fps: %d", g->fps_value );
  RC_draw_text ( &g->display, &g->font, fps_value_str, 0, CANVAS_HEIGHT - 12 );


  // ibmps
  for ( int y = 0; y < g->ibmps[ g->ibmps_index ].height; ++y )
    for ( int x = 0; x < g->ibmps[ g->ibmps_index ].width; ++x )
  {
    int color = BMP_indexed_rgb_at ( &g->ibmps[ g->ibmps_index ], x, y );
    g->display.array [ g->display.width * y + ( x + 64 ) ] = color;
  }




  ///////////////////////
  // Flush
  ///////////////////////

  uint8_t *pixels;
  int pitch;
  SDL_LockTexture ( g->canvas, NULL, ( void** ) &pixels, &pitch );

  for ( int i = 0; i < g->display.width * g->display.height; ++i ) {
    const uint32_t rgb = g->display.array [ i ];
    *( pixels + 0 ) = ( rgb >> 0x10 ) & 0xff;
    *( pixels + 1 ) = ( rgb >> 0x08 ) & 0xff;
    *( pixels + 2 ) = ( rgb >> 0x00 ) & 0xff;
    pixels += 3;
  }

  SDL_UnlockTexture ( g->canvas );
  SDL_RenderCopy ( g->renderer, g->canvas, NULL, NULL );
  SDL_RenderPresent ( g->renderer );

  return 0;
}



int
kill ( game_t *g ) {

  for ( int i = 0; i < g->ibmps_length; ++i ) BMP_indexed_kill ( &g->ibmps[ i ] );
  free ( g->ibmps );
  free ( g->itexs );


  BMP_font_kill ( &g->font );
  RC_kill_map ( &g->map );
  RC_kill_display ( &g->display );
  RC_kill_textures ( g->textures_length, g->textures );

  SDL_DestroyRenderer ( g->renderer );
  SDL_DestroyWindow ( g->window );
  SDL_Quit ();

  return 0;
}




