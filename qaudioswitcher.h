#ifndef QAUDIOSWITCHER_H
#define QAUDIOSWITCHER_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>

#include "pulseaudiosink.h"
#include "pulseaudiosinklistwidgetitem.h"

namespace Ui {
    class QAudioSwitcher;
}
class PulseAudioSinksManager;
class QAudioSwitcher : public QMainWindow
{
        Q_OBJECT

    public:
        explicit QAudioSwitcher(QWidget *parent = 0);
        ~QAudioSwitcher();
    public slots:
        void quit();
        void showError(QString message);
        void addDevice(PulseAudioSink sink);
        void sinkListComplete();
        void sinkSelectionChanged(QListWidgetItem* item);
        Q_SCRIPTABLE void switchSink();
        void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    private:
        void saveSinksList();
        void loadSinksList();
        void loadDefaultSink();
        void saveDefaultSink();
        void setupTrayIcon();
    private:
        Ui::QAudioSwitcher *ui;
        PulseAudioSinksManager *sinksManager;
        bool sinkListLoadingComplete;
        QString currentDefaultSink;
        QSystemTrayIcon* trayIcon;
        QMenu *trayIconMenu;
};

#endif // QAUDIOSWITCHER_H
