#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include <QQuickWindow>

class FramelessWindow : public QQuickWindow
{
    Q_OBJECT

    /*Q_PROPERTY(bool movable READ movable WRITE setMovable NOTIFY movableChanged)
    Q_PROPERTY(bool resizable READ resizable WRITE setResizable NOTIFY resizableChanged)*/

    enum MouseArea {
        TopLeft = 1,
        Top,
        TopRight,
        Left,
        Middle,
        Right,
        ButtomLeft,
        Buttom,
        ButtomRight
    };

public:
    explicit FramelessWindow(QWindow *parent = nullptr);
    ~FramelessWindow() override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    MouseArea getArea(const QPoint &pos);
    inline void setWindowGeometry(const QPoint &pos);
    void setCursorIcon();

    MouseArea m_currentArea = Middle;
    QPoint m_startPos;
    QPoint m_oldPos;
    QSize m_oldSize;
};

#endif
