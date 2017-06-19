#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>

// framebuffer + window class

class FrameBuffer : public Fl_Gl_Window {
public:
  unsigned int *pix; // SW color buffer
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
  // set one pixel function, check for frame boundaries
  void SetSafe(int u, int c, unsigned int color);
  // set one pixel function
  void Set(int u, int c, unsigned int color);
  // set to checkboard
  void SetCheckerboard(int checkerSize, unsigned int color0, 
    unsigned int color1);
  // draw circle
  void DrawCircle(float cuf, float cvf, float radius, unsigned int color);

};

