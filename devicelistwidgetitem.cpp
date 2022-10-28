#include "devicelistwidgetitem.h"
#include <QDebug>

DeviceListWidgetItem::DeviceListWidgetItem(AudioDevice device, QListWidget *view):
    QListWidgetItem(device.getDescription(),view,QListWidgetItem::Type), device(std::move(device))
{
    QIcon icon = QIcon::fromTheme(device.getIconName());
    if(icon.isNull()) {
        QString anotherIconName(device.getIconName().left(device.getIconName().lastIndexOf('-')));
        icon = QIcon::fromTheme(anotherIconName);
        qDebug()<<"icon name:"<<anotherIconName<<" , has theme icon : "<<QIcon::hasThemeIcon(anotherIconName);
    }
    if(icon.isNull()) {
        //we're still null, quess we need to load a standard icon then?
        icon = QIcon::fromTheme("audio-card");
    }

    qDebug()<<"icon name:"<<device.getIconName()<<" , has theme icon : "<<QIcon::hasThemeIcon(device.getIconName());
    setIcon(icon);
}

AudioDevice DeviceListWidgetItem::getDevice() const
{
    return device;
}
