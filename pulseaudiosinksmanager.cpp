#include "pulseaudiosinksmanager.h"
#include "qaudioswitcher.h"
#include "paoperation.h"
#include <pulse/pulseaudio.h>
#include <pulse/ext-stream-restore.h>
#include <QObject>
#include <QDebug>

auto context_deleter = [](pa_context* ctx){
    pa_context_disconnect(ctx);
    pa_context_unref(ctx);
};
auto loop_deleter = [](pa_threaded_mainloop* loop){
    pa_threaded_mainloop_free(loop);
};

PulseAudioSinksManager::PulseAudioSinksManager(QObject *parent):AudioManager(parent),
    pa_ml(nullptr,loop_deleter),pa_ctx(nullptr,context_deleter)
{
   qRegisterMetaType<AudioDevice>("AudioDevice");
}

PulseAudioSinksManager::~PulseAudioSinksManager()
{    
}

void PulseAudioSinksManager::retrieveSinks()
{
    pa_mainloop_api *pa_mlapi;
    // Create a mainloop API and connection to the default server
    pa_ml.reset(pa_threaded_mainloop_new());
    pa_mlapi = pa_threaded_mainloop_get_api(pa_ml.get());
    pa_ctx.reset(pa_context_new(pa_mlapi, "QAudioSwitcher"));
    pa_context_set_state_callback(pa_ctx.get(), PulseAudioSinksManager::pulseAudioStateCallback, this);
    pa_context_connect(pa_ctx.get(), NULL, PA_CONTEXT_NOFAIL  , NULL);
    pa_threaded_mainloop_start(pa_ml.get());
}

void PulseAudioSinksManager::pulseAudioMixerControlStreamRestoreCallback (pa_context *ctx, const pa_ext_stream_restore_info  *info,
                                     int eol, void *userdata) {
    pa_ext_stream_restore_info new_info;
    PulseAudioSinksManager* sinksManager = (PulseAudioSinksManager*)userdata;
    if (eol) return;

    new_info.name = info->name;
    new_info.channel_map = info->channel_map;
    new_info.volume = info->volume;
    new_info.mute = info->mute;
    std::string device = sinksManager->defaultDevice.toStdString();
    new_info.device = device.c_str();

    PaOperation op(pa_ext_stream_restore_write (ctx,PA_UPDATE_REPLACE,
                                     &new_info, 1,1, NULL, NULL));
    if (!op) {
            //g_warning ("pa_ext_stream_restore_write() failed: %s",
              //         pa_strerror (pa_context_errno (control->priv->pa_context)));
            return;
    }

    //g_debug ("Changed default device for %s to %s", info->name, info->device);

}

void PulseAudioSinksManager::setDefaultSink(const QString& name)
{

    defaultDevice = name;
    std::string std_name = defaultDevice.toStdString();
    qDebug() << "setting sink to "<<name<<", and from std_String to  "<<std_name.c_str();

    PaOperation op(pa_context_set_default_sink (pa_ctx.get(),std_name.c_str(),NULL,NULL));
    if (!op) {
            //g_warning ("pa_context_set_default_sink() failed: %s",
            //         pa_strerror (pa_context_errno (control->priv->pa_context)));
            return;
    }
    op = pa_ext_stream_restore_read (pa_ctx.get(), PulseAudioSinksManager::pulseAudioMixerControlStreamRestoreCallback,
                                    this);
    if (!op) {
        return;
    }    
}

void PulseAudioSinksManager::pulseAudioStateCallback(pa_context *ctx, void *userdata)
{
    PulseAudioSinksManager* sinksManager = (PulseAudioSinksManager*)userdata;
    pa_context_state_t state =  pa_context_get_state(ctx);
    switch  (state) {
    // There are just here for reference
    case PA_CONTEXT_UNCONNECTED:
    case PA_CONTEXT_CONNECTING:
    case PA_CONTEXT_AUTHORIZING:
    case PA_CONTEXT_SETTING_NAME:
    case PA_CONTEXT_TERMINATED:
    default:
        break;
    case PA_CONTEXT_FAILED:
        sinksManager->emitSignalError("Cannot connect to pulse audio");
        break;
    case PA_CONTEXT_READY:
        sinksManager->retrieveSinksInfo();
        break;
    }
}

void PulseAudioSinksManager::emitSignalError(const QString message) 
{
    emit signalError(message);
}

void PulseAudioSinksManager::emitSignalAddDevice(AudioDevice sink)
{
    emit signalAddDevice(sink);
}

void PulseAudioSinksManager::emitSignalSinkListComplete()
{
    emit signalSinkListComplete();
}

void PulseAudioSinksManager::pulseAudioSinklistCallback(pa_context* /*ctx*/, const pa_sink_info *info, int eol, void *userdata)
{
    PulseAudioSinksManager* sinksManager = (PulseAudioSinksManager*)userdata;
    // If eol is set to a positive number, you're at the end of the list
    if (eol == 0) {
        AudioDevice sink(info->name,info->description, sinksManager->getPulseAudioIconName(info->proplist).c_str());
        sinksManager->emitSignalAddDevice(sink);
    } else {
        sinksManager->emitSignalSinkListComplete();
    }
}

std::string PulseAudioSinksManager::getPulseAudioIconName(pa_proplist *properties)
{
    const char* name;
    if((name=pa_proplist_gets(properties,PA_PROP_DEVICE_ICON_NAME))) {
        return std::string(name);
    }
    if((name=pa_proplist_gets(properties,PA_PROP_MEDIA_ICON_NAME))) {
        return std::string(name);
    }
    if((name=pa_proplist_gets(properties,PA_PROP_WINDOW_ICON_NAME))) {
        return std::string(name);
    }
    if((name=pa_proplist_gets(properties,PA_PROP_APPLICATION_ICON_NAME))) {
        return std::string(name);
    }
    return "";
}

void PulseAudioSinksManager::retrieveSinksInfo()
{
    PaOperation op;
    op=pa_context_get_sink_info_list(pa_ctx.get(),PulseAudioSinksManager::pulseAudioSinklistCallback,this);
}
