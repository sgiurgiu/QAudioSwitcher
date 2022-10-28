#include "audiodevice.h"

AudioDevice::AudioDevice()
{

}

AudioDevice::AudioDevice(const QString &name, const QString &desc,
                               const QString &iconName)
    :name(name),description(desc),iconName(iconName)
{

}

void AudioDevice::setName(const QString &name)
{
    this->name = name;
}

void AudioDevice::setDescription(const QString &desc)
{
    this->description = desc;
}

void AudioDevice::setIconName(const QString &iconName)
{
    this->iconName = iconName;
}

QString AudioDevice::getName() const
{
    return name;
}

QString AudioDevice::getDescription() const
{
    return description;
}

QString AudioDevice::getIconName() const
{
    return iconName;
}
