#include "qaudioswitcher.h"
#include <QApplication>
#include <QtDBus/QtDBus>
#include <QMessageBox>
#include <QString>

#define SERVICE_NAME    "com.zergiu.QAudioSwitcher.SwitchAudio"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto bus = QDBusConnection::sessionBus();
    if(!bus.isConnected()) {
        QMessageBox dialog;
        dialog.critical(NULL,"Error","Cannot connect to DBus");
        return 1;
    }


    if (!bus.registerService(SERVICE_NAME)) {        
        auto lastError = bus.lastError();
        if(lastError.type() == QDBusError::NoError) return 1;
        QString msg;
        msg.append("DBus register error: ");
        msg.append(lastError.type());
        msg.append(",");
        msg.append(lastError.name());
        msg.append(",");
        msg.append(lastError.message());
        QMessageBox dialog;
        dialog.critical(NULL,"Error",msg);
        return 1;
    }

    QAudioSwitcher w;
    //ExportScriptableSlots
    QDBusConnection::sessionBus().registerObject("/", &w, QDBusConnection::ExportScriptableSlots);



    w.hide();

    return a.exec();
}
