import QtQuick 2.12

/*
 *             ↑     ↑     ↑
 *          ←\1\   \2\   \3\→
 *          ←\4\   \5\   \6\→
 *          ←\7\   \8\   \9\→
 *            ↓     ↓     ↓
 *           分8个缩放区域
 *           \5\为拖动区域
 *      target       缩放目标
 *      每一个area   大小 range x range
 *      startPoint   鼠标起始点
 *      fixedPont    用于固定窗口的点
*/

Item
{
    id: area

    property var target: undefined;
    property int range: 8
    property point startPoint: Qt.point(0, 0);
    property point fixedPont: Qt.point(0, 0);

    MouseArea {
        id: area1
        x: 0
        y: 0
        width: range
        height: range
        hoverEnabled: true

        onEntered: cursorShape = Qt.SizeFDiagCursor;
        onExited: cursorShape = Qt.ArrowCursor;
        onPressed: startPoint = Qt.point(mouseX, mouseY);
        onPositionChanged: {
            if(pressed) {
                var offsetX = mouse.x - startPoint.x;
                var offsetY = mouse.y - startPoint.y;
                //如果本次调整小于最小限制，则调整为最小
                if ((target.width - offsetX) >= target.minimumWidth) {
                    target.width -= offsetX;
                    target.x += offsetX;
                } else {
                    target.x += (target.width - target.minimumWidth);
                    target.width -= (target.width - target.minimumWidth);
                }

                if ((target.height - offsetY) >= target.minimumHeight && offsetY !== 0) {
                    target.height -= offsetY;
                    target.y += offsetY;
                }
            }
        }
    }

    MouseArea {
        id: area2
        x: range
        y: 0
        width: target.width - range * 2
        height: range
        hoverEnabled: true

        onEntered: cursorShape = Qt.SizeVerCursor;
        onExited: cursorShape = Qt.ArrowCursor;
        onPressed: startPoint = Qt.point(mouseX, mouseY);
        onPositionChanged: {
            if(pressed) {
                var offsetY = mouse.y - startPoint.y;
                if ((target.height - offsetY) >= target.minimumHeight && offsetY !== 0) {
                    target.height -= offsetY;
                    target.y += offsetY;
                }
            }
        }
    }

    MouseArea {
        id: area3 
        x: target.width - range
        y: 0
        width: range
        height: range
        hoverEnabled: true

        onEntered: cursorShape = Qt.SizeBDiagCursor;
        onExited: cursorShape = Qt.ArrowCursor
        onPressed:
        {
            startPoint = Qt.point(mouseX, mouseY);
            fixedPont = Qt.point(target.x, target.y)
        }
        onPositionChanged:
        {
            if(pressed)
            {
                var offsetX = mouse.x - startPoint.x;
                var offsetY = mouse.y - startPoint.y;
                if ((target.width + offsetX) >= target.minimumWidth && offsetX !== 0)
                {
                    target.width += offsetX;
                    target.x = fixedPont.x;
                }
                if ((target.height - offsetY) >= target.minimumHeight && offsetY !== 0)
                {
                    target.height -= offsetY;
                    target.y += offsetY;
                }
            }
        }
    }

    MouseArea {
        id: area4
        x: 0
        y: range
        width: range
        height: target.height - range * 2
        hoverEnabled: true

        onEntered: cursorShape = Qt.SizeHorCursor;
        onExited: cursorShape = Qt.ArrowCursor;
        onPressed: startPoint = Qt.point(mouseX, mouseY);
        onPositionChanged: {
            if(pressed) {
                var offsetX = mouse.x - startPoint.x;
                if ((target.width - offsetX) >= target.minimumWidth) {
                    target.width -= offsetX;
                    target.x += offsetX;
                }
            }
        }
    }

    MouseArea {
        id: area5
        x: range
        y: range
        width: area.target.width - range * 2
        height: area.target.height - range * 2
        target: area.target

        property var target: undefined;
        property point startPoint: Qt.point(0, 0);
        property point offsetPoint: Qt.point(0, 0);

        onPressed: {
            cursorShape = Qt.SizeAllCursor;
            startPoint = Qt.point(mouseX, mouseY);
        }
        onPositionChanged: {
            if(pressed) {
                offsetPoint = Qt.point(mouse.x - startPoint.x, mouse.y - startPoint.y);
                target.x = target.x + offsetPoint.x;
                target.y = target.y + offsetPoint.y;
            }
        }
        onReleased: cursorShape = Qt.ArrowCursor;
    }

    MouseArea {
        id: area6
        x: target.width - range
        y: range
        width: range
        height: target.height - range * 2
        hoverEnabled: true
        onEntered: cursorShape = Qt.SizeHorCursor;
        onExited: cursorShape = Qt.ArrowCursor;
        onPressed: {
            startPoint = Qt.point(mouseX, mouseY);
            fixedPont = Qt.point(target.x, target.y)
        }
        onPositionChanged:
        {
            if(pressed) {
                var offsetX = mouse.x - startPoint.x;
                if ((target.width + offsetX) >= target.minimumWidth && offsetX !== 0) {
                    target.width += offsetX;
                    target.x = fixedPont.x;
                }
            }
        }
    }

    MouseArea {
        id: area7
        x: 0
        y: target.height - range
        width: range
        height: target.height - range * 2
        hoverEnabled: true
        onEntered: cursorShape = Qt.SizeBDiagCursor;
        onExited: cursorShape = Qt.ArrowCursor;
        onPressed: {
            startPoint = Qt.point(mouseX, mouseY);
            fixedPont = Qt.point(target.x, target.y)
        }
        onPositionChanged: {
            if (pressed) {
                var offsetX = mouse.x - startPoint.x;
                var offsetY = mouse.y - startPoint.y;
                if ((target.width - offsetX) >= target.minimumWidth && offsetX !== 0) {
                    target.width -= offsetX;
                    target.x += offsetX;
                }
                if ((target.height + offsetY) >= target.minimumHeight && offsetY !== 0) {
                    target.height += offsetY;
                    target.y = fixedPont.y;
                }
            }
        }
    }

    MouseArea {
        id: area8
        x: range
        y: target.height - range
        width: target.height - range * 2
        height: range
        hoverEnabled: true
        onEntered: cursorShape = Qt.SizeVerCursor;
        onExited: cursorShape = Qt.ArrowCursor;
        onPressed: {
            startPoint = Qt.point(mouseX, mouseY);
            fixedPont = Qt.point(target.x, target.y)
        }
        onPositionChanged: {
            if (pressed) {
                var offsetY = mouse.y - startPoint.y;
                if ((target.height + offsetY) >= target.minimumHeight && offsetY !== 0) {
                    target.height += offsetY;
                    target.y = fixedPont.y;
                }
            }
        }
    }

    MouseArea {
        id: area9
        x: target.width - range
        y: target.height - range
        width: range
        height: range
        hoverEnabled: true

        onEntered: cursorShape = Qt.SizeFDiagCursor;
        onExited: cursorShape = Qt.ArrowCursor
        onPressed: {
            startPoint = Qt.point(mouseX, mouseY);
            fixedPont = Qt.point(target.x, target.y)
        }
        onPositionChanged: {
            if(pressed) {
                var offsetX = mouse.x - startPoint.x;
                var offsetY = mouse.y - startPoint.y;
                if ((target.width + offsetX) >= target.minimumWidth && offsetX !== 0) {
                    target.width += offsetX;
                    target.x = fixedPont.x;
                }
                if ((target.height + offsetY) >= target.minimumHeight && offsetY !== 0) {
                    target.height += offsetY;
                    target.y = fixedPont.y;
                }
            }
        }
    }
}

