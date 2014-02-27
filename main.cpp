#include "qaudioswitcher.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QAudioSwitcher w;
    w.show();

    return a.exec();
}
