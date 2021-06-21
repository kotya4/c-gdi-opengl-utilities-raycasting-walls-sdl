#ifndef EXT
#define EXT

#include <wingdi.h> // wglGetProcAddress
#include <string.h> // strstr
#include <gl/gl.h> // glGetString, GL_EXTENSIONS
#include <gl/glext.h> // PFNGLDRAWRANGEELEMENTSPROC

// opengl 1.3 extentions

PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements = NULL;

// get adresses of all extentions i need
int
EXT_load () {
  const char *ext_s = glGetString ( GL_EXTENSIONS );
  if ( 0 == strstr ( ext_s, ( "GL_EXT_draw_range_elements" ) ) ) return -1;
  glDrawRangeElements = ( PFNGLDRAWRANGEELEMENTSPROC )
    wglGetProcAddress ( "glDrawRangeElements" );
  return 0;
}

#endif // EXT