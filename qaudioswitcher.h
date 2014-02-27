#ifndef QAUDIOSWITCHER_H
#define QAUDIOSWITCHER_H

#include <QMainWindow>

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
        void showError(const QString message);
        void addDevice(const PulseAudioSink sink);
        void sinkListComplete();
        void sinkSelectionChanged(QListWidgetItem* item);
    private:
        Ui::QAudioSwitcher *ui;
        PulseAudioSinksManager *sinksManager;
};

#endif // QAUDIOSWITCHER_H
