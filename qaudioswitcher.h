#ifndef QAUDIOSWITCHER_H
#define QAUDIOSWITCHER_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QListWidgetItem>

#include "audiodevice.h"

namespace Ui {
    class QAudioSwitcher;
}
#ifdef PULSEAUDIO
//#define AudioManager PulseAudioSinksManager
#else
#error "AudioManager implementation not defined"
#endif

class AudioManager;

class QAudioSwitcher : public QMainWindow
{
        Q_OBJECT

    public:
        explicit QAudioSwitcher(QWidget *parent = 0);
        ~QAudioSwitcher();
    public slots:
        void quit();
        void showError(QString message);
        void addDevice(AudioDevice sink);
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
        AudioManager *audioManager;
        bool sinkListLoadingComplete;
        QString currentDefaultSink;
        QSystemTrayIcon* trayIcon;
        QMenu *trayIconMenu;
};

#endif // QAUDIOSWITCHER_H
