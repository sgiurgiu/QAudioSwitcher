#ifndef AUDIOMANAGERFACTORY_H
#define AUDIOMANAGERFACTORY_H

#include <QObject>

class AudioManager;

class AudioManagerFactory
{
public:
    static AudioManager* CreateAudioManager(QObject* parent = nullptr);
};

#endif // AUDIOMANAGERFACTORY_H
