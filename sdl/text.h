// Wrapper for SDL_ttf to render static TTF text into OpenGL texture.
# ifndef TEXT_H
# define TEXT_H

#include <stdio.h>
#include <gl/glu.h> // gluOrtho2D
#include <gl/gl.h>
#include "SDL_ttf.h"

TTF_Font
*TEXT_create_font ( const char *ttfpath, int ptsize ) {
  return TTF_OpenFont ( ttfpath, ptsize );
  // Returns: a pointer to the font as a TTF_Font. NULL is returned on errors.
  // source: https://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf_14.html
}

SDL_Surface
*TEXT_create_surface ( TTF_Font *font, const char *text, SDL_Color color ) {
  return TTF_RenderText_Blended ( font, text, color );
  // Returns: a pointer to a new SDL_Surface. NULL is returned on errors.
  // source: https://www.libsdl.org/projects/docs/SDL_ttf/SDL_ttf_44.html
}

GLuint
TEXT_create_texture ( SDL_Surface *surface ) {
  GLuint texture;
  glGenTextures ( 1, &texture );
  glBindTexture ( GL_TEXTURE_2D, texture );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexImage2D (
    GL_TEXTURE_2D,
    0,
    GL_RGBA,
    surface->w,
    surface->h,
    0,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    (unsigned char *)surface->pixels
  );
  // Makes sure that surface size is power of two ( GL_VERSION < 3 )
  GLenum err = glGetError ();
  if ( err != GL_NO_ERROR ) {
    fprintf ( stderr, "Error on TEXT_create_texture: %d\n", err );
    fprintf ( stderr, "Probably surface size ( %d, %d ) is not power of two.", surface->w, surface->h );
  }
  return texture;
}

void
TEXT_kill_font ( TTF_Font *font ) {
  TTF_CloseFont ( font );
}

void
TEXT_kill_surface ( SDL_Surface *surface ) {
  SDL_FreeSurface ( surface );
}

void
TEXT_kill_texture ( GLuint texture ) {
  glDeleteTextures ( 1, &texture );
}

void
TEXT_render ( GLuint texture, GLfloat texw, GLfloat texh, GLfloat x, GLfloat y, GLdouble ctxw, GLdouble ctxh ) {

  // Setup

  glDisable ( GL_DEPTH_TEST );
  glEnable ( GL_TEXTURE_2D );
  glEnable ( GL_BLEND );
  glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  // Matrices

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity ();
  gluOrtho2D ( 0.0, ctxw, ctxh, 0.0 );

  glMatrixMode ( GL_MODELVIEW );
  glLoadIdentity ();

  // Draw

  glBegin ( GL_QUADS );
  glTexCoord2f ( 0.f, 0.f ); glVertex2f ( x,        y );
  glTexCoord2f ( 1.f, 0.f ); glVertex2f ( x + texw, y );
  glTexCoord2f ( 1.f, 1.f ); glVertex2f ( x + texw, y + texh );
  glTexCoord2f ( 0.f, 1.f ); glVertex2f ( x,        y + texh );
  glEnd ();
}

# endif // TEXT_H
