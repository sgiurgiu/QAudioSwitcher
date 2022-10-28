#ifndef PULSEAUDIOSINKSMANAGER_H
#define PULSEAUDIOSINKSMANAGER_H

#include <QObject>
#include <memory>
#include <functional>
#include "audiodevice.h"
#include "audiomanager.h"

struct pa_context;
struct pa_threaded_mainloop;
struct pa_sink_info;
struct pa_ext_stream_restore_info;
struct pa_proplist;

class PulseAudioSinksManager : public AudioManager
{
    Q_OBJECT
    public:
        PulseAudioSinksManager(QObject* parent = 0);
        ~PulseAudioSinksManager();
        void retrieveSinks();
        void setDefaultSink(const QString &name);
    private:
        static void pulseAudioStateCallback(pa_context *ctx, void *userdata);
        static void pulseAudioSinklistCallback(pa_context *ctx, const pa_sink_info *info, int eol, void *userdata);
        void retrieveSinksInfo();
        static void pulseAudioMixerControlStreamRestoreCallback (pa_context *c, const pa_ext_stream_restore_info *info,int eol, void *userdata);
        std::string getPulseAudioIconName(pa_proplist* properties);
        void emitSignalError(const QString message);
        void emitSignalAddDevice(AudioDevice sink);
        void emitSignalSinkListComplete();
        
    private:        
        QString defaultDevice;

        std::unique_ptr<pa_threaded_mainloop ,std::function<void(pa_threaded_mainloop*)>> pa_ml;
        std::unique_ptr<pa_context,std::function<void(pa_context*)>> pa_ctx;
};

#endif // PULSEAUDIOSINKSMANAGER_H
