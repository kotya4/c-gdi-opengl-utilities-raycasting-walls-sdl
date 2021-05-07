// Simple qube renderer using OpenGL fixed function pipeline for my old SiS Mirage.
#ifndef FFQUBE_H
#define FFQUBE_H

#include <gl/glu.h> // gluPerspective
#include <gl/gl.h>

void
FFQUBE_render () {
  // Rotation
  static GLfloat rx = 0.f;
  static GLfloat ry = 0.f;
  static GLfloat rz = 0.f;
  rx += 0.10f * 10;
  ry -= 0.05f * 10;
  rz += 0.15f * 10;

  // Setup

  glEnable ( GL_DEPTH_TEST );
  glDisable ( GL_TEXTURE_2D );

  // Matrices

  glMatrixMode ( GL_PROJECTION );
  glLoadIdentity ();
  gluPerspective ( 45.0, 1.0, 0.1, 1000.0 );

  glMatrixMode ( GL_MODELVIEW );
  glLoadIdentity ();
  glTranslatef ( 0.0f, 0.0f, -7.0f );
  glRotatef ( rx, 1.f, 0.f, 0.f );
  glRotatef ( ry, 0.f, 1.f, 0.f );
  glRotatef ( rz, 0.f, 0.f, 1.f );

  // Draw

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

  glFlush ();
}

#endif // FFQUBE_H
