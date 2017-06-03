#ifndef PULSEAUDIOSINK_H
#define PULSEAUDIOSINK_H
#include <QObject>
#include <QString>

class PulseAudioSink
{

    public:
        PulseAudioSink();
        PulseAudioSink(const QString& name,const QString& desc, const QString& iconName);
        ~PulseAudioSink(){ }

        void setName(const QString& name);
        void setDescription(const QString& desc);
        void setIconName(const QString& iconName);
        QString getName() const;
        QString getDescription() const;
        QString getIconName() const;
    private:
        QString name;
        QString description;
        QString iconName;
};

#endif // PULSEAUDIOSINK_H
