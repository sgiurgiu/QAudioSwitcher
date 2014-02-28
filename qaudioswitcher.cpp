#include "qaudioswitcher.h"
#include <QDesktopWidget>
#include <QMessageBox>
#include <QDebug>
#include <QTextStream>
#include <QDir>
#include <QFile>
#include "ui_qaudioswitcher.h"
#include "pulseaudiosinksmanager.h"


QAudioSwitcher::QAudioSwitcher(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QAudioSwitcher),sinksManager(new PulseAudioSinksManager(this)),
    sinkListLoadingComplete(false)
{
    ui->setupUi(this);
    ui->centralWidget->setLayout(ui->verticalLayout);
    QObject::connect(ui->listWidget,SIGNAL(itemChanged(QListWidgetItem*)),this, SLOT(sinkSelectionChanged(QListWidgetItem*)));
    this->move(QApplication::desktop()->availableGeometry().center() - this->rect().center());
    QDir::home().mkdir(".qaudioswitcher");
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
    loadSinksList();
    sinkListLoadingComplete = true;
}

void QAudioSwitcher::sinkSelectionChanged(QListWidgetItem *item)
{
    if(!sinkListLoadingComplete) return;
    PulseAudioSinkListWidgetItem* sinkItem = dynamic_cast<PulseAudioSinkListWidgetItem*>(item);
    if(!sinkItem) {
        return;
    }
    qDebug()<< "Item selection changed "<<sinkItem->getSink().getDescription()<<", with check state "<<sinkItem->checkState();
    saveSinksList();
}


//we can afford to do this since we won't have too many
void QAudioSwitcher::saveSinksList()
{
    QDir propDir(QDir::home().path()+QDir::separator()+".qaudioswitcher");
    QFile propFile(propDir.filePath("sinks"));
    propFile.open(QIODevice::WriteOnly);
    QTextStream propFileStream(&propFile);
    int itemCount = ui->listWidget->count();
    for(int i=0;i<itemCount; ++i) {
        PulseAudioSinkListWidgetItem* sinkItem = dynamic_cast<PulseAudioSinkListWidgetItem*>(ui->listWidget->item(i));
        if(!sinkItem) {
            continue;
        }
        if(sinkItem->checkState() == Qt::Checked) {
            propFileStream << sinkItem->getSink().getName() << "\r\n";
        }
    }
    propFile.close();
}

void QAudioSwitcher::loadSinksList()
{
    QDir propDir(QDir::home().path()+QDir::separator()+".qaudioswitcher");
    QFile propFile(propDir.filePath("sinks"));
    if(propFile.exists()) {
        propFile.open(QIODevice::ReadOnly);
        QTextStream propFileStream(&propFile);
        QString line;
        while(!propFileStream.atEnd() && !(line = propFileStream.readLine()).isNull()) {
            int itemCount = ui->listWidget->count();
            for(int i=0;i<itemCount;++i) {
                PulseAudioSinkListWidgetItem* sinkItem = dynamic_cast<PulseAudioSinkListWidgetItem*>(ui->listWidget->item(i));
                if(!sinkItem) {
                    continue;
                }
                if(sinkItem->getSink().getName() == line) {
                    sinkItem->setCheckState(Qt::Checked);
                }
            }
        }
    } else {
        propFile.open(QIODevice::WriteOnly);
    }
    propFile.close();
}
