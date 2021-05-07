// Sample SDL2 + OpenGL (GLU + GLEW) program

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <gl/glew.h>
#include <gl/glu.h>
#include <gl/gl.h>
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_opengl.h"
#include "ffqube.h"
#include "text.h"

typedef int bool_t;
enum { false = 0, true = 1 };

#define nextpow2_u32( x ) ( 1 << ( 32 - __builtin_clz ( x - 1 ) ) )

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 400

int
main ( int argc, char **args ) {
  if ( SDL_Init ( SDL_INIT_VIDEO ) < 0 ) {
    fprintf (stderr, "SDL_Init: %s\n", SDL_GetError () );
    return 1;
  }

  if ( TTF_Init () < 0 ) {
    fprintf (stderr, "TTF_Init: %s\n", SDL_GetError () );
    return 1;
  }

  uint32_t window_flags = SDL_WINDOW_OPENGL;

  SDL_Window *window = SDL_CreateWindow (
    "hello_sdl2",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    SCREEN_WIDTH, SCREEN_HEIGHT,
    window_flags
	);

  if ( window == NULL ) {
    fprintf ( stderr, "SDL_CreateWindow: %s\n", SDL_GetError () );
    return 1;
  }

  SDL_GL_SetAttribute ( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
  SDL_GL_SetAttribute ( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
  SDL_GL_SetAttribute ( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

  SDL_GLContext context = SDL_GL_CreateContext ( window );

  if ( context == NULL ) {
    fprintf ( stderr, "SDL_GL_CreateContext: %s\n", SDL_GetError () );
    return 1;
  }

  if ( SDL_GL_MakeCurrent (window, context) < 0 ) {
    fprintf ( stderr, "SDL_GL_MakeCurrent: %s\n", SDL_GetError () );
    return 1;
  }

  printf ( "GL_VERSION: %s\n", glGetString ( GL_VERSION ) );
  // printf ( "GL_EXTENSIONS: %s\n", glGetString ( GL_EXTENSIONS ) );

  glewExperimental = GL_TRUE;
  GLenum glew_init_status = glewInit ();
  if ( glew_init_status != GLEW_OK ) {
    fprintf ( stderr, "glewInit: %s\n", glewGetErrorString ( glew_init_status ) );
    return 1;
  }

  printf ( "GLEW_VERSION: %s\n", glewGetString ( GLEW_VERSION ) );

  // Tip: On my SiS Mirage GL_VERSION is 1.14.0, tho
  //      glewIsSupported says GL_VERSION_1_4 is not supported.

  if ( glewIsSupported ( "GL_VERSION_1_3" ) ) {
    printf ( "glewIsSupported: GL_VERSION_1_3 supported\n" );
  } else {
    printf ( "glewIsSupported: GL_VERSION_1_3 NOT supported\n" );
  }

  if ( glewIsSupported ( "GL_VERSION_1_4" ) ) {
    printf ( "glewIsSupported: GL_VERSION_1_4 supported\n" );
  } else {
    printf ( "glewIsSupported: GL_VERSION_1_4 NOT supported\n" );
  }

  // TTF text to GL texture

  TTF_Font *font = TTF_OpenFont ( "RobotoMono-Regular.ttf", 20 );
  if ( font == NULL ) {
    fprintf ( stderr, "TTF_OpenFont: %s\n", SDL_GetError () );
    return 1;
  }

  SDL_Surface *sur = TTF_RenderText_Blended ( font, "Hello, Wordl!", ( SDL_Color ){ 64, 128, 255 } );
  if ( sur == NULL ) {
    fprintf ( stderr, "TTF_RenderText_Blended: %s\n", SDL_GetError () );
    return 1;
  }

  const int tex_w = nextpow2_u32 ( sur->w );
  const int tex_h = nextpow2_u32 ( sur->h );
  if ( sur->w != tex_w || sur->h != tex_h ) {
    // Surface size need to be power of two ( GL_VERSION < 3 ),
    // so we create new surface with proper size and copy pixels
    // from old surface.
    const int depth = 32;
    const SDL_PixelFormatEnum pf = SDL_PIXELFORMAT_RGBA32;
    SDL_Surface *new_sur = SDL_CreateRGBSurfaceWithFormat ( 0, tex_w, tex_h, depth, pf );
    if ( new_sur == NULL ) {
      fprintf ( stderr, "SDL_CreateRGBSurfaceWithFormat: %s\n", SDL_GetError () );
      return 1;
    }
    if ( SDL_BlitSurface ( sur, &sur->clip_rect, new_sur, &new_sur->clip_rect ) < 0 ) {
      fprintf ( stderr, "SDL_BlitSurface: %s\n", SDL_GetError () );
      return 1;
    }
    // Swap pointers
    SDL_FreeSurface ( sur );
    sur = new_sur;
  }

  GLuint tex = TEXT_create_texture ( sur );

  // Setup

  glViewport ( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
  glClearColor ( 0.5f, 0.5f, 0.5f, 0.f );

  // Main Loop

  bool_t is_running = true;
  bool_t is_fullscreen = false;

  while ( is_running ) {

    // Events

    SDL_Event event;
    while ( SDL_PollEvent ( &event ) ) {

      // Keyboard

      if ( event.type == SDL_KEYDOWN ) {
        // Escape
        if ( event.key.keysym.sym == SDLK_ESCAPE ) {
          is_running = false;
        }
        // F key
        else if ( event.key.keysym.sym == 'f' ) {
          is_fullscreen = !is_fullscreen;
          SDL_SetWindowFullscreen (
            window,
            is_fullscreen ? window_flags | SDL_WINDOW_FULLSCREEN_DESKTOP : window_flags
          );
          // TODO: need reinit context or smth
        }
      }

      // Quit

      else if ( event.type == SDL_QUIT ) {
        is_running = false;
      }
    }

    // Render

    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    FFQUBE_render ();

    TEXT_render ( tex, sur->w, sur->h, 30, 30, SCREEN_WIDTH, SCREEN_HEIGHT );

    SDL_GL_SwapWindow ( window );
  }

  // Destroy

  glDeleteTextures ( 1, &tex );
  SDL_FreeSurface ( sur );
  TTF_CloseFont ( font );
  SDL_GL_DeleteContext ( context );
  SDL_DestroyWindow ( window );
  SDL_Quit ();
  return 0;
}
