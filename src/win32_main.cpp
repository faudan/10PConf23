#include "win32_main.h"

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <xinput.h>

#include "definitions.h"
#include "10pconf.cpp"
#include "10pconf.h"
#include "stdio.h"

#define arrayCount(array) (sizeof(array) / sizeof((array)[0]))


global Win32OffscreenBuffer globalBackbuffer;
global char running;

uint32 KM_UP = 'W';
uint32 KM_DOWN = 'S';
uint32 KM_LEFT = 'A';
uint32 KM_RIGHT = 'D';

internal Win32WindowDimension
getWindowDimension(HWND windowHandle) {
  RECT clientRect;
  GetClientRect(windowHandle, &clientRect);
  Win32WindowDimension windowDimension;
  windowDimension.width = clientRect.right - clientRect.left;
  windowDimension.height = clientRect.bottom - clientRect.top;
  return windowDimension;
}

internal void
win32ResizeDeviceIndependentBitmapSection(Win32OffscreenBuffer *buffer, int width, int height) {
  if (buffer->memory) {
    VirtualFree(buffer->memory, 0, MEM_RELEASE);
  }

  buffer->width = width;
  buffer->height = height;

  BITMAPINFOHEADER bmiHeader = {0};
  bmiHeader.biSize = sizeof(bmiHeader);
  bmiHeader.biWidth = width;
  bmiHeader.biHeight = -height;
  bmiHeader.biPlanes = 1;
  bmiHeader.biBitCount = 32;
  bmiHeader.biCompression = BI_RGB;
  buffer->info.bmiHeader = bmiHeader;

  int bytesPerPixel = 4;
  int bitmapMemorySize = bytesPerPixel * (width * height);

  buffer->memory = VirtualAlloc(0, bitmapMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

  buffer->pitch = bytesPerPixel * width;
}

inline internal void win32DisplayBufferInWindow(HDC deviceContext, Win32OffscreenBuffer *buffer, int windowWidth, int windowHeight) {
  StretchDIBits(deviceContext,
                0, 0, windowWidth, windowHeight,
                0, 0, buffer->width, buffer->height,
                buffer->memory,
                &(buffer->info),
                DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK win32MainWindowCallback(
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam) {
  LRESULT result = 0;

  switch (message) {
    case WM_SIZE: {
      OutputDebugStringA("Message resize\n");
    } break;
    case WM_DESTROY: {
      OutputDebugStringA("Message destroy\n");
    } break;
    case WM_CLOSE: {
      PostQuitMessage(0);
      running = 0;
      OutputDebugStringA("Message close\n");
    } break;
    case WM_ACTIVATEAPP: {
      OutputDebugStringA("Message activateapp\n");
    } break;
    case WM_PAINT: {
      PAINTSTRUCT paint;
      HDC deviceContext = BeginPaint(window, &paint);
      RECT rect = paint.rcPaint;

      Win32WindowDimension windowDimension = getWindowDimension(window);

      win32DisplayBufferInWindow(deviceContext, &globalBackbuffer, windowDimension.width, windowDimension.height);

      EndPaint(window, &paint);

      OutputDebugStringA("Message paint\n");
    } break;

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP: {
    } break;

    default: {
      result = DefWindowProc(window, message, wParam, lParam);
    } break;
  }

  return result;
}


internal void setButtonState(ButtonState *button, bool32 isDown) {
  button->isPressed = isDown;
  button->halfTransitionCount++;
}

internal void handleKeyboardInput(Keyboard *keyboard, WPARAM wParam, LPARAM lParam) {
  uint32 vkCode = (uint32)wParam;

  bool32 keyWasDown = ((1 << 30) & lParam) != 0;
  bool32 keyIsDown = ((1 << 31) & lParam) == 0;
  bool32 altIsPressed = ((1 << 29) & lParam);

  if (keyWasDown != keyIsDown) {
    if (vkCode == KM_UP || vkCode == VK_UP) {
      setButtonState(&keyboard->moveUp, keyIsDown);
    }
    if (vkCode == KM_LEFT || vkCode == VK_LEFT) {
      setButtonState(&keyboard->moveLeft, keyIsDown);
    }
    if (vkCode == KM_RIGHT || vkCode == VK_RIGHT) {
      setButtonState(&keyboard->moveRight, keyIsDown);
    }
    if (vkCode == KM_DOWN || vkCode == VK_DOWN) {
      setButtonState(&keyboard->moveDown, keyIsDown);
    }
    if (vkCode == VK_NUMPAD1 || vkCode == '1') {
      setButtonState(&keyboard->action2, keyIsDown);
    }
    if (vkCode == VK_NUMPAD2 || vkCode == '2') {
      setButtonState(&keyboard->action1, keyIsDown);
    }
    if (vkCode == VK_SPACE) {
      setButtonState(&keyboard->action4, keyIsDown);
    }
    if (vkCode == 'Q') {
      running = false;
    }
  }
  if (vkCode == VK_F4 && altIsPressed) {
    running = false;
  }
}

internal void handleMouseInput(MouseInput *mouseInput, WPARAM wParam, LPARAM lParam) {
  bool32 mouseLButtonIsDown = (wParam & MK_LBUTTON) == 1;

  mouseInput->leftClick.isPressed = mouseLButtonIsDown;
  mouseInput->x = GET_X_LPARAM(lParam); 
  mouseInput->y = GET_Y_LPARAM(lParam);
}

internal void win32HandleMessage(Keyboard *keyboard, MouseInput *mouseInput) {
  MSG message;

  while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
    switch (message.message) {
      case WM_QUIT: {
        running = 0;
      } break;

      case WM_SYSKEYDOWN:
      case WM_SYSKEYUP:
      case WM_KEYDOWN:
      case WM_KEYUP: {
        handleKeyboardInput(keyboard, message.wParam, message.lParam);
      } break;
      case WM_LBUTTONDOWN:
      case WM_LBUTTONUP:
      case WM_RBUTTONDOWN:
      case WM_RBUTTONUP:
      case WM_MOUSEMOVE: {
        handleMouseInput(mouseInput, message.wParam, message.lParam);
      } break;
      default:
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
  }
}

internal AppState* initializeState() {
  uint64 memorySize = sizeof(AppState);
  AppState *state = (AppState *) VirtualAlloc(0, (size_t)memorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

  state->red = 1;
  state->blue = 1;
  state->pos = {600,200};

  return state;
}


HWND windowSetup( HINSTANCE instance,
    HINSTANCE prevInstance,
    LPSTR commandLine,
    int showCode) {
  WNDCLASSA winClass = {0};

  int windowWidth = 1280;
  int windowHeight = 720;
  win32ResizeDeviceIndependentBitmapSection(&globalBackbuffer, windowWidth, windowHeight);

  winClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
  winClass.lpfnWndProc = win32MainWindowCallback;
  winClass.hInstance = instance;
  winClass.lpszClassName = "win-32-app-class";

  if (RegisterClassA(&winClass)) {
    HWND windowHandle = CreateWindowExA(
        0, winClass.lpszClassName, "10 Pines Conf",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, instance, 0);

    if (windowHandle) {
      return windowHandle;
    } else {
      OutputDebugStringA("failed CreateWindowEx");
    }

  } else {
    OutputDebugStringA("failed RegisterClass");
  }
  
  return 0;
}

void processInput(UserInput* currentInput, UserInput* previousInput) {
      Keyboard *keyboard = &(currentInput->keyboard);
      Keyboard *previousKeyboard = &(previousInput->keyboard);
      MouseInput *mouseInput = &(currentInput->mouse);
      MouseInput *previousMouseInput = &(previousInput->mouse);

      *keyboard = {};
      *mouseInput = {};
      mouseInput->x = previousMouseInput->x;
      mouseInput->y = previousMouseInput->y;
      mouseInput->leftClick.isPressed = previousMouseInput->leftClick.isPressed;

      for (int buttonIndex = 0; buttonIndex < arrayCount(keyboard->buttons); ++buttonIndex) {
        keyboard->buttons[buttonIndex].isPressed = previousKeyboard->buttons[buttonIndex].isPressed;
      }

      win32HandleMessage(keyboard, mouseInput);
}

int WINAPI WinMain(
    HINSTANCE instance,
    HINSTANCE prevInstance,
    LPSTR commandLine,
    int showCode) {    
  HWND windowHandle = windowSetup(instance, prevInstance, commandLine, showCode);
  if (windowHandle) {
    HDC deviceContext = GetDC(windowHandle);
    running = 1;
    SetCursor(NULL); 

    // Setup
    UserInput inputs[2] = {};
    UserInput *currentInput = &inputs[0];
    UserInput *previousInput = &inputs[1];
    AppState* state = initializeState();

    while (running) {
      OffScreenBuffer bitmapBuffer = {
          globalBackbuffer.memory,
          globalBackbuffer.width,
          globalBackbuffer.height,
          globalBackbuffer.pitch,
      };

      processInput(currentInput, previousInput);
      updateState(currentInput, state);
      renderFrame(&bitmapBuffer, state, currentInput);
      
      // Display frame
      Win32WindowDimension windowDimension = getWindowDimension(windowHandle);
      win32DisplayBufferInWindow(deviceContext, &globalBackbuffer, windowDimension.width, windowDimension.height);
      
      // Clean-up
      ReleaseDC(windowHandle, deviceContext);
      UserInput *temp = currentInput;
      currentInput = previousInput;
      previousInput = temp;
    }

    free(state);
  }

  return 0;
}
