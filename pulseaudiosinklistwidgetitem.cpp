#include "pulseaudiosinklistwidgetitem.h"
#include <QDebug>

PulseAudioSinkListWidgetItem::PulseAudioSinkListWidgetItem(const PulseAudioSink &sink, QListWidget *view):
    QListWidgetItem(sink.getDescription(),view,QListWidgetItem::Type), sink(sink)
{
    QIcon icon = QIcon::fromTheme(sink.getIconName());
    if(icon.isNull()) {
        QString anotherIconName(sink.getIconName().left(sink.getIconName().lastIndexOf('-')));
        icon = QIcon::fromTheme(anotherIconName);
        qDebug()<<"icon name:"<<anotherIconName<<" , has theme icon : "<<QIcon::hasThemeIcon(anotherIconName);
    }
    if(icon.isNull()) {
        //we're still null, quess we need to load a standard icon then?
        icon = QIcon::fromTheme("audio-card");
    }

    qDebug()<<"icon name:"<<sink.getIconName()<<" , has theme icon : "<<QIcon::hasThemeIcon(sink.getIconName());
    setIcon(icon);
}

PulseAudioSink PulseAudioSinkListWidgetItem::getSink() const
{
    return sink;
}
