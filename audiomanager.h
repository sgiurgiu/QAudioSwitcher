#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QObject>
#include <QString>
#include "audiodevice.h"

class AudioManager : public QObject
{
    Q_OBJECT
public:
    explicit AudioManager(QObject *parent = nullptr);
    virtual ~AudioManager() = default;
    virtual void retrieveSinks() = 0;
    virtual void setDefaultSink(const QString &name) = 0;

signals:
    void signalError(const QString message);
    void signalAddDevice(AudioDevice sink);
    void signalSinkListComplete();

};

#endif // AUDIOMANAGER_H
