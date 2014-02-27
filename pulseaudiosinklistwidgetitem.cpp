#include "pulseaudiosinklistwidgetitem.h"

PulseAudioSinkListWidgetItem::PulseAudioSinkListWidgetItem(const PulseAudioSink &sink, QListWidget *view):
    QListWidgetItem(sink.getDescription(),view,QListWidgetItem::Type), sink(sink)
{

}

PulseAudioSink PulseAudioSinkListWidgetItem::getSink() const
{
    return sink;
}
