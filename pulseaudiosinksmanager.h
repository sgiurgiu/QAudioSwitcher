#ifndef PULSEAUDIOSINKSMANAGER_H
#define PULSEAUDIOSINKSMANAGER_H
#include <QObject>
#include "pulseaudiosink.h"
class QAudioSwitcher;
struct pa_context;
struct pa_threaded_mainloop;
struct pa_sink_info;
struct pa_ext_stream_restore_info;
class PulseAudioSinksManager : public QObject
{
    Q_OBJECT
    public:
        PulseAudioSinksManager(QAudioSwitcher* appWindow);
        ~PulseAudioSinksManager();
        void retrieveSinks();
        void setDefaultSink(const QString &name);
    signals:
        void signalError(const QString message);
        void signalAddDevice(const PulseAudioSink sink);
        void signalSinkListComplete();
    private:        
        static void pulseAudioStateCallback(pa_context *ctx, void *userdata);
        static void pulseAudioSinklistCallback(pa_context *ctx, const pa_sink_info *info, int eol, void *userdata);
        void retrieveSinksInfo();
        static void pulseAudioMixerControlStreamRestoreCallback (pa_context *c, const pa_ext_stream_restore_info *info,
                                                                 int eol, void *userdata);
    private:
        QAudioSwitcher* appWindow;
        pa_context *pa_ctx;
        pa_threaded_mainloop *pa_ml;
        QString defaultDevice;

};

#endif // PULSEAUDIOSINKSMANAGER_H
