#include "qaudioswitcher.h"
#include <QApplication>
#include <QtDBus/QtDBus>
#include <QMessageBox>
#define SERVICE_NAME    "com.zergiu.QAudioSwitcher.SwitchAudio"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(!QDBusConnection::sessionBus().isConnected()) {
        QMessageBox dialog;
        dialog.critical(NULL,"Error","Cannot connect to DBus");
        return 1;
    }


    if (!QDBusConnection::sessionBus().registerService(SERVICE_NAME)) {
        QMessageBox dialog;
        dialog.critical(NULL,"Error",qPrintable(QDBusConnection::sessionBus().lastError().message()));
        return 1;
    }

    QAudioSwitcher w;
    //ExportScriptableSlots
    QDBusConnection::sessionBus().registerObject("/", &w, QDBusConnection::ExportScriptableSlots);


    w.hide();

    return a.exec();
}
