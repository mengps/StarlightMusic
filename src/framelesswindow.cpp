#include "framelesswindow.h"

FramelessWindow::FramelessWindow(QWindow *parent)
    : QQuickWindow (parent)

{
    setFlags(flags() | Qt::Window | Qt::FramelessWindowHint);
    setColor(Qt::transparent);
}

FramelessWindow::~FramelessWindow()
{

}

