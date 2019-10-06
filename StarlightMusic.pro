QT += quick multimedia

CONFIG += c++11 debug_and_release

CONFIG(debug, debug|release) {
    DEFINES += NO_BUFFER_OUTPUT
    DESTDIR = $$shell_path(./debug)
} else {
    DEFINES += NO_BUFFER_OUTPUT
    DESTDIR = $$shell_path(./release)
}

INCLUDEPATH += $$PWD/ffmpeg/include

LIBS += -L$$PWD/ffmpeg/lib \
        -lavcodec \
        -lavformat \
        -lavutil \
        -lswscale \
        -lswresample

win32 {
    ffmpeg_dll = $$shell_path($$PWD/ffmpeg/dll)
    QMAKE_POST_LINK = \
        copy $$ffmpeg_dll $$DESTDIR
}

RC_ICONS += image/music.ico

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    src/audiodecoder.h \
    src/bufferqueue.h \
    src/fileapi.h \
    src/imageprovider.h \
    src/lrcdecoder.h \
    src/lyricsmodel.h \
    src/musicmodel.h \
    src/musicplayer.h \
    src/skinmanager.h

SOURCES += \
    src/audiodecoder.cpp \
    src/fileapi.cpp \
    src/imageprovider.cpp \
    src/lrcdecoder.cpp \
    src/lyricsmodel.cpp \
    src/main.cpp \
    src/musicmodel.cpp \
    src/musicplayer.cpp \
    src/skinmanager.cpp

RESOURCES += \
    qml.qrc \
    image.qrc \
    skin.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
