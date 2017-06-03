#include "pulseaudiosink.h"

PulseAudioSink::PulseAudioSink():name(""),description(""),iconName("")
{

}

PulseAudioSink::PulseAudioSink(const QString &name, const QString &desc,
                               const QString &iconName)
    :name(name),description(desc),iconName(iconName)
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

void PulseAudioSink::setIconName(const QString &iconName)
{
    this->iconName = iconName;
}

QString PulseAudioSink::getName() const
{
    return name;
}

QString PulseAudioSink::getDescription() const
{
    return description;
}

QString PulseAudioSink::getIconName() const
{
    return iconName;
}
