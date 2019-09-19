#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include <QQuickWindow>

class FramelessWindow : public QQuickWindow
{
    Q_OBJECT

public:
    explicit FramelessWindow(QWindow *parent = nullptr);
    ~FramelessWindow();
};

#endif
