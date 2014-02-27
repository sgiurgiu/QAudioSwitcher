#include "pulseaudiosinksmanager.h"
#include "qaudioswitcher.h"
#include <pulse/pulseaudio.h>
#include <QObject>
#include <QMetaType>

PulseAudioSinksManager::PulseAudioSinksManager(QAudioSwitcher* appWindow):
    appWindow(appWindow)
{
    qRegisterMetaType<PulseAudioSink>("PulseAudioSink");
    QObject::connect(this, SIGNAL(signalError(const QString)),
                          appWindow, SLOT(showError(const QString)));
    QObject::connect(this, SIGNAL(signalAddDevice(const PulseAudioSink)),
                          appWindow, SLOT(addDevice(const PulseAudioSink)));


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
    if (eol > 0) {

        return;
    }
    char description[512]={0};
    char name[512]={0};
    strncpy(description, info->description, 511);
    strncpy(name, info->name, 511);
    PulseAudioSink sink(name,description);
    sinksManager->signalAddDevice(sink);
}

void PulseAudioSinksManager::retrieveSinksInfo()
{
    pa_operation *pa_op;
    pa_op = pa_context_get_sink_info_list(pa_ctx,PulseAudioSinksManager::pulseAudioSinklistCallback,this);

}
