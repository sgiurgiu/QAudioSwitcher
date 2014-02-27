#ifndef PULSEAUDIOSINK_H
#define PULSEAUDIOSINK_H
#include <QString>

class PulseAudioSink
{

    public:
        PulseAudioSink();
        PulseAudioSink(const QString& name,const QString& desc);        
        ~PulseAudioSink(){ }

        void setName(const QString& name);
        void setDescription(const QString& desc);
        QString getName() const;
        QString getDescription() const;
    private:
        QString name;
        QString description;
};

#endif // PULSEAUDIOSINK_H
