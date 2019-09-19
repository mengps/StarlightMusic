#include "framelesswindow.h"
#include "imageprovider.h"
#include "musicplayer.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qmlRegisterType<LyricData>("an.lyrics", 1, 0, "LyricData");
    qmlRegisterType<LyricsModel>("an.lyrics", 1, 0, "LyricsModel");
    qmlRegisterType<FramelessWindow>("an.window", 1, 0, "FramelessWindow");

    //给它一个parent
    MusicPlayer *musicPlayer = new MusicPlayer(qApp);
    QQmlApplicationEngine engine;
    engine.addImageProvider("playbill", musicPlayer->imageProvider());
    engine.rootContext()->setContextProperty("musicPlayer", musicPlayer);
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
