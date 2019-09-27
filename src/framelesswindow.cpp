#include "framelesswindow.h"

FramelessWindow::FramelessWindow(QWindow *parent)
    : QQuickWindow (parent)

{
    setFlags(flags() | Qt::Window | Qt::FramelessWindowHint);
}

FramelessWindow::~FramelessWindow()
{

}

void FramelessWindow::mousePressEvent(QMouseEvent *event)
{
    m_startPos = event->globalPos();
    m_oldPos = position();
    m_oldSize = size();

    QQuickWindow::mousePressEvent(event);
}

void FramelessWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_oldPos = position();
    m_oldSize = size();

    QQuickWindow::mouseReleaseEvent(event);
}

void FramelessWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (m_currentArea == Middle) {
        if (windowState() == Qt::WindowMaximized)
            showNormal();
        else if (windowState() == Qt::WindowNoState)
            showMaximized();
    }

    QQuickWindow::mouseDoubleClickEvent(event);
}

void FramelessWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        //单独处理移动区域，这样可以更快
        if (m_currentArea == Middle) {
            setPosition(m_oldPos - (m_startPos - event->globalPos()));
        } else {
            setWindowGeometry(event->globalPos());
        }
    } else {
        QPoint pos = event->pos();
        m_currentArea = getArea(pos);
        setCursorIcon();
    }

    QQuickWindow::mouseMoveEvent(event);
}

FramelessWindow::MouseArea FramelessWindow::getArea(const QPoint &pos)
{
    int x = pos.x();
    int y = pos.y();
    int w = width();
    int h = height();
    MouseArea area;

    if (x >= 0 && x <= 8 && y >= 0 && y <= 8) {
        area = TopLeft;
    } else if (x > 8 && x < (w - 8) && y >= 0 && y <= 8) {
        area = Top;
    } else if (x >=(w - 8) && x <= w && y >= 0 && y <= 8) {
        area = TopRight;
    } else if (x >= 0 && x <= 8 && y > 8 && y < (h - 8)) {
        area = Left;
    } else if (x >=(w - 8) && x <= w && y > 8 && y < (h - 8)) {
        area = Right;
    } else if (x >= 0 && x <= 8 && y >= (h - 8) && y <= h) {
        area = ButtomLeft;
    } else if (x > 8 && x < (w - 8) && y >= (h - 8) && y <= h) {
        area = Buttom;
    } else if (x >=(w - 8) && x <= w && y >= (h - 8) && y <= h) {
        area = ButtomRight;
    } else {
        area = Middle;
    }

    return area;
}

void FramelessWindow::setWindowGeometry(const QPoint &pos)
{
    QPoint offset = m_startPos - pos;

    if (offset.x() == 0 || offset.y() == 0)
        return;

    static auto set_geometry = [this](const QSize &size, const QPoint &pos) {
        QPoint temp_pos = m_oldPos;
        QSize temp_size = m_oldSize;
        if (size.width() > minimumWidth()) {
            temp_pos.setX(pos.x());
            temp_size.setWidth(size.width());
        }
        if (size.height() > minimumHeight()) {
            temp_pos.setY(pos.y());
            temp_size.setHeight(size.height());
        }
        setPosition(temp_pos);
        resize(temp_size);
        update();
    };

    switch (m_currentArea) {
    case TopLeft:
        set_geometry(m_oldSize + QSize(offset.x(), offset.y()), m_oldPos - offset);
        break;
    case Top:
        set_geometry(m_oldSize + QSize(0, offset.y()), m_oldPos - QPoint(0, offset.y()));
        break;
    case TopRight:
        set_geometry(m_oldSize - QSize(offset.x(), -offset.y()), m_oldPos - QPoint(0, offset.y()));
        break;
    case Left:
        set_geometry(m_oldSize + QSize(offset.x(), 0), m_oldPos - QPoint(offset.x(), 0));;
        break;
    case Right:
        set_geometry(m_oldSize - QSize(offset.x(), 0), position());
        break;
    case ButtomLeft:
        set_geometry(m_oldSize + QSize(offset.x(), -offset.y()), m_oldPos - QPoint(offset.x(), 0));
        break;
    case Buttom:
        set_geometry(m_oldSize + QSize(0, -offset.y()), position());
        break;
    case ButtomRight:
        set_geometry(m_oldSize - QSize(offset.x(), offset.y()), position());
        break;
    default:
        break;
    }

}

void FramelessWindow::setCursorIcon()
{
    switch (m_currentArea) {
    case TopLeft:
    case ButtomRight:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case Top:
    case Buttom:
        setCursor(Qt::SizeVerCursor);
        break;
    case TopRight:
    case ButtomLeft:
        setCursor(Qt::SizeBDiagCursor);
        break;
    case Left:
    case Right:
        setCursor(Qt::SizeHorCursor);
        break;
    case Middle:
        setCursor(Qt::ArrowCursor);
        break;
    }
}

