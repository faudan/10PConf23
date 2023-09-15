#ifndef CONF10P_H
#include "definitions.h"


struct OffScreenBuffer {
  void *memory;
  int width;
  int height;
  int pitch;
};

struct ButtonState {
  int32 halfTransitionCount;
  bool32 isPressed;
};

struct Keyboard {
  union {
    ButtonState buttons[8];
    struct {
      ButtonState moveUp;
      ButtonState moveDown;
      ButtonState moveLeft;
      ButtonState moveRight;

      ButtonState action1;
      ButtonState action2;
      ButtonState action3;
      ButtonState action4;
    };
  };
};

struct MouseInput {
  int32 x, y;
  ButtonState leftClick;
};

struct UserInput {
  MouseInput mouse;
  Keyboard keyboard;
};

struct V2 {
  int x,y;
};


struct AppState {
  int blue;
  int red;
  V2 pos;
};


void updateAndRender(OffScreenBuffer *bitmapBuffer, UserInput *input, AppState *state);


#define CONF10P_H
#endif // CONF10P_H