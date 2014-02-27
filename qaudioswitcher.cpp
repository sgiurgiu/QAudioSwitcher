#include "qaudioswitcher.h"
#include <QDesktopWidget>
#include <QMessageBox>
#include "ui_qaudioswitcher.h"
#include "pulseaudiosinksmanager.h"


QAudioSwitcher::QAudioSwitcher(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QAudioSwitcher),sinksManager(new PulseAudioSinksManager(this))
{
    ui->setupUi(this);
    this->move(QApplication::desktop()->availableGeometry().center() - this->rect().center());
    sinksManager->retrieveSinks();
}

QAudioSwitcher::~QAudioSwitcher()
{
    delete ui;
    delete sinksManager;
}

void QAudioSwitcher::showError(const QString message)
{
    QMessageBox dialog;
    dialog.critical(this,"Error",message);
}

void QAudioSwitcher::addDevice(const PulseAudioSink sink)
{
    PulseAudioSinkListWidgetItem* item=new PulseAudioSinkListWidgetItem(sink,ui->listWidget);
    item->setCheckState(Qt::Unchecked);
    ui->listWidget->addItem(item);
}

void QAudioSwitcher::sinkListComplete()
{

}

void QAudioSwitcher::sinkSelectionChanged(QListWidgetItem *item)
{

}
