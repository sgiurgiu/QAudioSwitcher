#include "pulseaudiosink.h"

PulseAudioSink::PulseAudioSink():name(""),description("")
{

}

PulseAudioSink::PulseAudioSink(const QString &name, const QString &desc):name(name),description(desc)
{

}

void PulseAudioSink::setName(const QString &name)
{
    this->name = name;
}

void PulseAudioSink::setDescription(const QString &desc)
{
    this->description = desc;
}

QString PulseAudioSink::getName() const
{
    return name;
}

QString PulseAudioSink::getDescription() const
{
    return description;
}
