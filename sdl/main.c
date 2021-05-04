// Sample SDL2 + OpenGL (GLU + GLEW) program

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <gl/glew.h>
#include <gl/glu.h>
#include <gl/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 400

void draw_rotating_quad ( GLfloat, GLfloat, GLfloat );

int
main ( int argc, char **args ) {
  if ( SDL_Init ( SDL_INIT_VIDEO ) < 0 ) {
    fprintf (stderr, "SDL_Init: %s\n", SDL_GetError () );
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

  // Tip: My GL_VERSION is 1.14.0, but glewIsSupported says GL_VERSION_1_4 is not supported.

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

  // Set up scene

  glEnable ( GL_DEPTH_TEST );

  glViewport ( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
  glClearColor ( 0.5f, 0.5f, 0.5f, 0.f );

  int is_running = 1;
  int is_fullscreen = 0;

  GLfloat angle_x = 0.f;
  GLfloat angle_y = 0.f;
  GLfloat angle_z = 0.f;

  while ( is_running ) {

    // Events

    SDL_Event event;
    while ( SDL_PollEvent ( &event ) ) {

      // Keyboard

      if ( event.type == SDL_KEYDOWN ) {
        // Escape
        if ( event.key.keysym.sym == SDLK_ESCAPE ) {
          is_running = 0;
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
        is_running = 0;
      }
    }

    // Render

    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    angle_x += 0.10f * 10;
    angle_y -= 0.05f * 10;
    angle_z += 0.15f * 10;
    draw_rotating_quad ( angle_x, angle_y, angle_z );

    glFlush ();

    SDL_GL_SwapWindow ( window );
  }

  // Destroy
  SDL_GL_DeleteContext ( context );
  SDL_DestroyWindow ( window );
  SDL_Quit ();
  return 0;
}


void
draw_rotating_quad ( GLfloat rx, GLfloat ry, GLfloat rz ) {
  glMatrixMode ( GL_PROJECTION );
  glLoadIdentity ();
  gluPerspective ( 45.0, 1.0, 0.1, 1000.0 );

  glMatrixMode ( GL_MODELVIEW );
  glLoadIdentity ();
  glTranslatef ( 0.0f, 0.0f, -7.0f );
  glRotatef ( rx, 1.f, 0.f, 0.f );
  glRotatef ( ry, 0.f, 1.f, 0.f );
  glRotatef ( rz, 0.f, 0.f, 1.f );

  // Define vertices in counter-clockwise (CCW) order with normal pointing out
  glBegin ( GL_QUADS );
  // Top face (y = 1.0f)
  // Green
  glColor3f ( 0.0f, 1.0f, 0.0f );
  glVertex3f ( +1.0f, +1.0f, -1.0f );
  glVertex3f ( -1.0f, +1.0f, -1.0f );
  glVertex3f ( -1.0f, +1.0f, +1.0f );
  glVertex3f ( +1.0f, +1.0f, +1.0f );
  // Bottom face (y = -1.0f)
  // Orange
  glColor3f (1.0f, 0.5f, 0.0f );
  glVertex3f ( +1.0f, -1.0f, +1.0f );
  glVertex3f ( -1.0f, -1.0f, +1.0f );
  glVertex3f ( -1.0f, -1.0f, -1.0f );
  glVertex3f ( +1.0f, -1.0f, -1.0f );
  // Front face  (z = 1.0f)
  // Red
  glColor3f ( 1.0f, 0.0f, 0.0f );
  glVertex3f ( +1.0f, +1.0f, +1.0f );
  glVertex3f ( -1.0f, +1.0f, +1.0f );
  glVertex3f ( -1.0f, -1.0f, +1.0f );
  glVertex3f ( +1.0f, -1.0f, +1.0f );
  // Back face (z = -1.0f)
  // Yellow
  glColor3f ( 1.0f, 1.0f, 0.0f );
  glVertex3f ( +1.0f, -1.0f, -1.0f );
  glVertex3f ( -1.0f, -1.0f, -1.0f );
  glVertex3f ( -1.0f, +1.0f, -1.0f );
  glVertex3f ( +1.0f, +1.0f, -1.0f );
  // Left face (x = -1.0f)
  // Blue
  glColor3f ( 0.0f, 0.0f, 1.0f );
  glVertex3f ( -1.0f, +1.0f, +1.0f );
  glVertex3f ( -1.0f, +1.0f, -1.0f );
  glVertex3f ( -1.0f, -1.0f, -1.0f );
  glVertex3f ( -1.0f, -1.0f, +1.0f );
  // Right face (x = 1.0f)
  // Magenta
  glColor3f ( 1.0f, 0.0f, 1.0f );
  glVertex3f ( +1.0f, +1.0f, -1.0f );
  glVertex3f ( +1.0f, +1.0f, +1.0f );
  glVertex3f ( +1.0f, -1.0f, +1.0f );
  glVertex3f ( +1.0f, -1.0f, -1.0f );
  // End of drawing color-cube
  glEnd ();
}