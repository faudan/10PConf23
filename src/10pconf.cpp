#include "definitions.h"
#include "10pconf.h"
#include <math.h>

internal 
void renderGradient(OffScreenBuffer *buffer, int xOffset, int yOffset) {
  uint8 *row = (uint8 *)buffer->memory;

  int bitmapHeight = buffer->height;
  int bitmapWidth = buffer->width;

  for (int y = 0; y < bitmapHeight; y++) {
    uint32 *pixel = (uint32 *)row;
    for (int x = 0; x < bitmapWidth; x++) {
      // Pixel = BB GG RR ¿AA?

      uint32 blue = ((uint8)((x + yOffset) * 255 / bitmapWidth)) << 0;
      uint32 green = ((uint8)((y + yOffset) * 255 / bitmapHeight)) << 8;
      uint32 red = ((uint8)((bitmapWidth - (x + yOffset)) * 255 / bitmapWidth)) << 16;
      uint32 alpha = ((uint8)(y * 255 / bitmapHeight)) << 24;
      *pixel = red | green | blue | alpha;
      pixel++;
    }
    row += buffer->pitch;
  }
}

internal 
void renderGradientRB(OffScreenBuffer *buffer, int redFlag, int blueFlag) {
  uint8 *row = (uint8 *)buffer->memory;

  int bitmapHeight = buffer->height;
  int bitmapWidth = buffer->width;

  for (int y = 0; y < bitmapHeight; y++) {
    uint32 *pixel = (uint32 *)row;
    for (int x = 0; x < bitmapWidth; x++) {
      // Pixel = BB GG RR ¿AA?

      uint32 blue = ((uint8)(((y) * 255 * blueFlag) / bitmapHeight)) << 0;
      uint32 green = 00 << 8;
      uint32 red = ((uint8)(((bitmapWidth - (x)) * 255) * redFlag / bitmapWidth)) << 16;
      uint32 alpha = 0xF << 24;
      *pixel = red | green | blue | alpha;
      pixel++;
    }
    row += buffer->pitch;
  }
}

bool32 between(int min, int max, int value) {
  return (value >= min) && (value <= max);
}

bool32 positionInBounds(int xMin, int yMin, int xMax, int yMax, int x, int y) {
  return between(xMin, xMax, x) && between(yMin, yMax, y);
}

internal 
void renderButton(OffScreenBuffer *buffer, UserInput *input,  int xPos, int yPos, int width, int height, uint32 color, uint32 pressedColor, int* buttonState, AppState *state) {
  uint8 *row = (uint8 *)buffer->memory;

  int bitmapHeight = buffer->height;
  int bitmapWidth = buffer->width;
  MouseInput mouse = input->mouse;
  int xMax = xPos+width;
  int yMax = yPos+height;
  bool32 mouseOver = positionInBounds(xPos, yPos, xMax, yMax, mouse.x, mouse.y);
  bool32 playerOver = positionInBounds(xPos, yPos, xMax, yMax, state->pos.x, state->pos.y);
  if(mouseOver) {
    if(mouse.leftClick.isPressed) {
      *buttonState = !(*buttonState);
    }
  }
  if(playerOver) {
    if(input->keyboard.action4.isPressed) {
      *buttonState = !(*buttonState);
    }
  }


  for (int y = 0; y < bitmapHeight; y++) {
    uint32 *pixel = (uint32 *)row;
    for (int x = 0; x < bitmapWidth; x++) {

      bool32 inBounds = positionInBounds(xPos, yPos, xMax, yMax, x, y);
      if(inBounds) {
        if(*buttonState){
          *pixel = pressedColor;
        }else {
          *pixel = color;
        }
      }

      pixel++;
    }
    row += buffer->pitch;
  }

}

internal 
void renderMouse(OffScreenBuffer *buffer, UserInput *input) {
  uint8 *row = (uint8 *)buffer->memory;
  int bitmapHeight = buffer->height;
  int bitmapWidth = buffer->width;
  MouseInput mouse = input->mouse;
  int mouseRadius = 3;

  for (int y = 0; y < bitmapHeight; y++) {
    uint32 *pixel = (uint32 *)row;
    for (int x = 0; x < bitmapWidth; x++) {
      int inY = between(mouse.y-mouseRadius, mouse.y+mouseRadius, y);
      int inX = between(mouse.x-mouseRadius, mouse.x+mouseRadius, x);
      if(inY && inX) {
        *pixel = 0xFFFFFFFF;  
      }
      pixel++;
    }
    row += buffer->pitch;
  }
}


internal 
void renderRect(OffScreenBuffer *buffer, V2 position, V2 size, uint32 color ) {
  uint8 *row = (uint8 *)buffer->memory;
  int bitmapHeight = buffer->height;
  int bitmapWidth = buffer->width;
  int xPos = position.x;
  int yPos = position.y;
  int width = size.x;
  int height = size.y;

  for (int y = 0; y < bitmapHeight; y++) {
    uint32 *pixel = (uint32 *)row;
    for (int x = 0; x < bitmapWidth; x++) {
      int xMax = xPos+width;
      int yMax = yPos+height;
      bool32 inBounds = positionInBounds(xPos, yPos, xMax, yMax, x, y);
      if(inBounds) {
          *pixel = color;
        }
      pixel++;
    }
    row += buffer->pitch;
  }
}



void updateState( UserInput *input, AppState* state) {
    Keyboard gameInput = input->keyboard;
    if(gameInput.action2.isPressed) {
      state->red = !state->red;
    }
    if(gameInput.action1.isPressed) {
      state->blue = !state->blue;
    }
    if(gameInput.moveDown.isPressed) {
      state->pos.y += 5;
    }
    if(gameInput.moveUp.isPressed) {
      state->pos.y -= 5;
    }
    if(gameInput.moveLeft.isPressed) {
      state->pos.x -= 5;
    }
    if(gameInput.moveRight.isPressed) {
      state->pos.x += 5;
    }


}

void renderFrame(OffScreenBuffer* bitmapBuffer,AppState* state, UserInput *input){
    int bitmapHeight = bitmapBuffer->height;
    int bitmapWidth = bitmapBuffer->width;
    renderGradientRB(bitmapBuffer, state->red, state->blue);
    renderButton(bitmapBuffer, input, bitmapWidth*0.80, bitmapHeight/25, 50, 30, 0xFF0000, 0x880000,&state->red, state);
    renderButton(bitmapBuffer, input, bitmapWidth*0.80 + 60, bitmapHeight/25, 50, 30, 0xFF, 0x88 ,&state->blue, state);
    renderRect(bitmapBuffer, state->pos, {10,10}, 0x00FF00);
    renderMouse(bitmapBuffer, input);
}
