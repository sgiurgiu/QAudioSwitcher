#ifndef PULSEAUDIOSINKLISTWIDGETITEM_H
#define PULSEAUDIOSINKLISTWIDGETITEM_H

#include <QListWidgetItem>
#include "pulseaudiosink.h"

class PulseAudioSinkListWidgetItem :public QListWidgetItem
{
    public:
        PulseAudioSinkListWidgetItem(const PulseAudioSink& sink, QListWidget *view = 0);
        PulseAudioSink getSink() const;
    private:
        PulseAudioSink sink;
};

#endif // PULSEAUDIOSINKLISTWIDGETITEM_H
