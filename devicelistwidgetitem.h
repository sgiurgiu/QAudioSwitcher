#ifndef PULSEAUDIOSINKLISTWIDGETITEM_H
#define PULSEAUDIOSINKLISTWIDGETITEM_H

#include <QListWidgetItem>
#include "audiodevice.h"

class DeviceListWidgetItem :public QListWidgetItem
{
    public:
        DeviceListWidgetItem(AudioDevice device, QListWidget *view = 0);
        AudioDevice getDevice() const;
    private:
        AudioDevice device;
};

#endif // PULSEAUDIOSINKLISTWIDGETITEM_H
