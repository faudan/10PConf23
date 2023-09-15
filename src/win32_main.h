#ifndef WIN32_MAIN_H
#include "definitions.h"
#include <dsound.h>
#include <windows.h>
#include <xinput.h>

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

struct Win32SoundOutput {
  int samplesPerSecond;
  int bytesPerSample;
  int wavePeriod;
  int secondaryAudioBufferSize;
  int toneVolume;
  uint32 runningSampleIndex;
  int16 toneHz;
};

struct Win32SoundConfig {
  DWORD bytesToWrite;
  DWORD byteToLock;
  bool32 succeeded;
};



#define WIN32_MAIN_H
#endif // WIN32_MAIN_H