#ifndef CAPSLOCKINDICATOR_H_
#define CAPSLOCKINDICATOR_H_

#include <Windows.h>

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK LowLevelKeyboardProc(int, WPARAM, LPARAM);
void cleanup();

#endif // !CAPSLOCKINDICATOR_H_
