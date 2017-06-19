#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>

#include "TMesh.h"
class PPC;

// framebuffer + window class

class FrameBuffer : public Fl_Gl_Window {
public:
  unsigned int *pix; // SW color buffer
  float *zb; // zbuffer for visibility
  int w, h; // image resolution
  FrameBuffer(int u0, int v0, int _w, int _h); // constructor, top left coords and resolution

  // function that is always called back by system and never called directly by programmer
  // programmer triggers framebuffer update by calling FrameBuffer::redraw(), which makes
  //            system call draw
  void draw();

  // function called back when event occurs (mouse, keyboard, etc)
  int handle(int event);
  void KeyboardHandle();

  // clear to background color
  void Set(unsigned int color);
  // clear z buffer to far distance, corresponding to background
  void ClearZB(float farz);
  // set one pixel function, check for frame boundaries
  void SetSafe(int u, int c, unsigned int color);
  // set one pixel function
  void Set(int u, int c, unsigned int color);
  // sets pixel to color c if wins z test
  void SetIfCloser(V3 p, V3 c);
  // set to checkboard
  void SetCheckerboard(int checkerSize, unsigned int color0, 
    unsigned int color1);

  // draw circle
  void DrawCircle(float cuf, float cvf, float radius, unsigned int color);

  // draw TMesh in wire frame
  void DrawWireFrame(TMesh *tm, PPC *ppc);
  // draw vertex dots
  void DrawVertexDots(TMesh *tm, int dotSize, PPC *ppc);

  // draw 3D segment specified by 2 points, each with own color
  void Draw3DSegment(V3 v0, V3 c0, V3 v1, V3 c1, PPC *ppc);
  void Draw3DSegmentNaive(V3 v0, V3 c0, V3 v1, V3 c1, PPC *ppc);
  void Draw2DSegment(V3 v0, V3 c0, V3 v1, V3 c1);

  // Draw AABB
  void DrawAABB(AABB aabb, unsigned int colorNear, unsigned int colorFar,
    PPC *ppc);

  // save as tiff image
  void SaveAsTiff(char *fname);

};


