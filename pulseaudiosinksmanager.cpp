#include "pulseaudiosinksmanager.h"
#include "qaudioswitcher.h"
#include <pulse/pulseaudio.h>
#include <pulse/ext-stream-restore.h>
#include <QObject>
#include <QDebug>

PulseAudioSinksManager::PulseAudioSinksManager(QAudioSwitcher* appWindow):
    appWindow(appWindow)
{
    qRegisterMetaType<PulseAudioSink>("PulseAudioSink");
    QObject::connect(this, SIGNAL(signalError(const QString)),
                          appWindow, SLOT(showError(const QString)));
    QObject::connect(this, SIGNAL(signalAddDevice(const PulseAudioSink)),
                          appWindow, SLOT(addDevice(const PulseAudioSink)));
    QObject::connect(this, SIGNAL(signalSinkListComplete()),
                          appWindow, SLOT(sinkListComplete()));
}

PulseAudioSinksManager::~PulseAudioSinksManager()
{
    pa_context_disconnect(pa_ctx);
    pa_context_unref(pa_ctx);
    pa_threaded_mainloop_free(pa_ml);
}

void PulseAudioSinksManager::retrieveSinks()
{
    pa_mainloop_api *pa_mlapi;
    // Create a mainloop API and connection to the default server
    pa_ml = pa_threaded_mainloop_new();
    pa_mlapi = pa_threaded_mainloop_get_api(pa_ml);
    pa_ctx = pa_context_new(pa_mlapi, "QAudioSwitcher");
    pa_context_set_state_callback(pa_ctx, PulseAudioSinksManager::pulseAudioStateCallback, this);
    pa_context_connect(pa_ctx, NULL, PA_CONTEXT_NOFLAGS , NULL);
    pa_threaded_mainloop_start(pa_ml);
}

void PulseAudioSinksManager::pulseAudioMixerControlStreamRestoreCallback (pa_context *c, const pa_ext_stream_restore_info  *info,
                                     int eol, void *userdata) {
    pa_ext_stream_restore_info new_info;
    pa_operation *o;

    PulseAudioSinksManager* sinksManager = (PulseAudioSinksManager*)userdata;
    if (eol) return;

    new_info.name = info->name;
    new_info.channel_map = info->channel_map;
    new_info.volume = info->volume;
    new_info.mute = info->mute;
    std::string device = sinksManager->defaultDevice.toStdString();
    new_info.device = device.c_str();

    o = pa_ext_stream_restore_write (sinksManager->pa_ctx,PA_UPDATE_REPLACE,
                                     &new_info, 1,TRUE, NULL, NULL);

    if (o == NULL) {
            //g_warning ("pa_ext_stream_restore_write() failed: %s",
              //         pa_strerror (pa_context_errno (control->priv->pa_context)));
            return;
    }

    //g_debug ("Changed default device for %s to %s", info->name, info->device);

    pa_operation_unref (o);
}

void PulseAudioSinksManager::setDefaultSink(const QString& name)
{
    pa_operation *pa_op;
    defaultDevice = name;
    std::string std_name = defaultDevice.toStdString();
    qDebug() << "setting sink to "<<name<<", and from std_String to  "<<std_name.c_str();

    pa_op = pa_context_set_default_sink (pa_ctx,std_name.c_str(),NULL,NULL);
    if (pa_op == NULL) {
            //g_warning ("pa_context_set_default_sink() failed: %s",
            //         pa_strerror (pa_context_errno (control->priv->pa_context)));
            return;
    }
    pa_op = pa_ext_stream_restore_read (pa_ctx, PulseAudioSinksManager::pulseAudioMixerControlStreamRestoreCallback,
                                    this);

    if (pa_op == NULL) {
        return;
    }
    pa_operation_unref (pa_op);
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
        sinksManager->signalError("Cannot connect to pulse audio");
        break;
    case PA_CONTEXT_READY:
        sinksManager->retrieveSinksInfo();
        break;
    }
}

void PulseAudioSinksManager::pulseAudioSinklistCallback(pa_context* /*ctx*/, const pa_sink_info *info, int eol, void *userdata)
{
    PulseAudioSinksManager* sinksManager = (PulseAudioSinksManager*)userdata;
    // If eol is set to a positive number, you're at the end of the list
    if (eol == 0) {
        char description[512]={0};
        char name[512]={0};
        strncpy(description, info->description, 511);
        strncpy(name, info->name, 511);        
        PulseAudioSink sink(name,description);
        sinksManager->signalAddDevice(sink);
    } else {
        sinksManager->signalSinkListComplete();
    }
}

void PulseAudioSinksManager::retrieveSinksInfo()
{
    pa_operation *pa_op;
    pa_op = pa_context_get_sink_info_list(pa_ctx,PulseAudioSinksManager::pulseAudioSinklistCallback,this);
    pa_operation_unref (pa_op);
}
