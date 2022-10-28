#include "audiomanagerfactory.h"

#ifdef PULSEAUDIO
#include "pulseaudiosinksmanager.h"
#else
#error "AudioManager implementation not defined"
#endif

AudioManager* AudioManagerFactory::CreateAudioManager(QObject* parent)
{
#ifdef PULSEAUDIO
    return new PulseAudioSinksManager(parent);
#else
    return nullptr;
#endif
}
