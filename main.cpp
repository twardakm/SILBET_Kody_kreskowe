#include "mainwindow.h"
#include "keyboard_hook.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed (true);
    MainWindow w;
    w.show();

    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, MyLowLevelKeyBoardProc, NULL, 0);
    if (hHook == NULL)
    {
        qDebug() << "Hook Failed" << endl;
    }

    return a.exec();
}
