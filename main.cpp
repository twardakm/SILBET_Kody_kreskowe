#include "mainwindow.h"
#include <QApplication>
#include <windows.h>

#define LINE_FEED '\r'

#pragma comment(lib, "user32.lib")

HHOOK hHook = NULL;
MainWindow *address;

using namespace std;

void UpdateKeyState(BYTE *keystate, int keycode)
{
    keystate[keycode] = GetKeyState(keycode);
}

//this code is from http://stackoverflow.com/questions/2167876/is-it-possible-to-use-win32-hooks-in-qt-applications
//with some my changes

LRESULT CALLBACK MyLowLevelKeyBoardProc(int nCode, WPARAM wParam, LPARAM lParam)
{ //WPARAM is WM_KEYDOWn, WM_KEYUP, WM_SYSKEYDOWN, or WM_SYSKEYUP //LPARAM is the key information
    if (wParam == WM_KEYDOWN)
    {
        //Get the key information
        KBDLLHOOKSTRUCT cKey = *((KBDLLHOOKSTRUCT*)lParam);

        wchar_t buffer[5];

        //get the keyboard state
        BYTE keyboard_state[256];
        GetKeyboardState(keyboard_state);
        UpdateKeyState(keyboard_state, VK_SHIFT);
        UpdateKeyState(keyboard_state, VK_CAPITAL);
        UpdateKeyState(keyboard_state, VK_CONTROL);
        UpdateKeyState(keyboard_state, VK_MENU);

        //Get keyboard layout
        HKL keyboard_layout = GetKeyboardLayout(0);

        DWORD dwMsg = 1;
        dwMsg += cKey.scanCode << 16;
        dwMsg += cKey.flags << 24;

        //Try to convert the key information
        ToUnicodeEx(cKey.vkCode, cKey.scanCode, keyboard_state, buffer, 4, 0, keyboard_layout);
        buffer[4] = L'\0';

        QString temp = QString::fromUtf16((ushort*)buffer);

        //Print the output
        qDebug() << "Key: " << temp;
        if (temp[0].isDigit())
            address->get_key(temp[0].digitValue());
        else if (temp[0] == QChar(LINE_FEED))
            address->get_line_feed();
    }

    return CallNextHookEx(hHook, nCode, wParam, lParam);
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed (true);
    static MainWindow w;
    w.show();
    address = &w;

    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, MyLowLevelKeyBoardProc, NULL, 0);
    if (hHook == NULL)
    {
        qDebug() << "Hook Failed" << endl; //close app
    }

    return a.exec();
}
