#include "fileapi.h"
#include "imageprovider.h"
#include "lyricsmodel.h"
#include "musicmodel.h"
#include "musicplayer.h"
#include "skinmanager.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qmlRegisterType<LyricData>("an.lyrics.model", 1, 0, "LyricData");
    qmlRegisterType<LyricsModel>("an.lyrics.model", 1, 0, "LyricsModel");
    qmlRegisterType<AudioData>("an.music.model", 1, 0, "AudioData");
    qmlRegisterType<MusicModel>("an.music.model", 1, 0, "MusicModel");
    qmlRegisterType<MusicPlayer>("an.music", 1, 0, "MusicPlayer");

    //给它一个parent
    MusicPlayer *musicPlayer = new MusicPlayer(qApp);
    QQmlApplicationEngine engine;
    engine.addImageProvider("playbill", musicPlayer->imageProvider());
    engine.rootContext()->setContextProperty("fileApi", new FileApi(qApp));
    engine.rootContext()->setContextProperty("musicPlayer", musicPlayer);
    engine.rootContext()->setContextProperty("skinManager", SkinManager::instance());
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
