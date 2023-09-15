#ifndef WIN32_MAIN_H
#include "definitions.h"
#include <windows.h>

struct Win32OffscreenBuffer {
  BITMAPINFO info;
  void *memory;
  int width;
  int height;
  int pitch;
};

struct Win32WindowDimension {
  int width;
  int height;
};


#define WIN32_MAIN_H
#endif // WIN32_MAIN_H