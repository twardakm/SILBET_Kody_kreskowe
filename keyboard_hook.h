#ifndef KEYBOARD_HOOK_H
#define KEYBOARD_HOOK_H

#include <windows.h>
#pragma comment(lib, "user32.lib")

HHOOK hHook = NULL;

using namespace std;

void UpdateKeyState(BYTE *keystate, int keycode);
//this code is from http://stackoverflow.com/questions/2167876/is-it-possible-to-use-win32-hooks-in-qt-applications
//with some my changes

LRESULT CALLBACK MyLowLevelKeyBoardProc(int nCode, WPARAM wParam, LPARAM lParam);

#endif // KEYBOARD_HOOK_H
