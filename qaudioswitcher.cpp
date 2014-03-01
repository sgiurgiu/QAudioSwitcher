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
    sinkListLoadingComplete(false),currentDefaultSink("")
{
    ui->setupUi(this);
    ui->centralWidget->setLayout(ui->verticalLayout);

//    QObject::connect(ui->hideButton,SIGNAL(clicked()),ui->action_Hide, SLOT(trigger()));
    QObject::connect(ui->listWidget,SIGNAL(itemChanged(QListWidgetItem*)),this, SLOT(sinkSelectionChanged(QListWidgetItem*)));
    QObject::connect(ui->switchButton,SIGNAL(clicked()),this, SLOT(switchSink()));
    this->move(QApplication::desktop()->availableGeometry().center() - this->rect().center());
    QDir::home().mkdir(".qaudioswitcher");
    setupTrayIcon();
    sinksManager->retrieveSinks();
}

QAudioSwitcher::~QAudioSwitcher()
{
    delete ui;
    delete sinksManager;
}

void QAudioSwitcher::setupTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(ui->action_Show);
    trayIconMenu->addAction(ui->action_Quit);


    QIcon icon("/usr/share/cinnamon-control-center/icons/hicolor/48x48/devices/audio-speaker-center.svg");
    trayIcon = new QSystemTrayIcon(icon, this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
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
    loadDefaultSink();
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

void QAudioSwitcher::switchSink()
{
    sinkListLoadingComplete = false;
    //TODO: make it less lame. it stinks.
    int itemCount = ui->listWidget->count();
    QList<QString> defaultSinksToUse;
    for(int i=0;i<itemCount;++i) {
        PulseAudioSinkListWidgetItem* sinkItem = dynamic_cast<PulseAudioSinkListWidgetItem*>(ui->listWidget->item(i));
        if(!sinkItem) {
            continue;
        }
        if(sinkItem->checkState() == Qt::Checked) {
            defaultSinksToUse.append(sinkItem->getSink().getName());
        }
    }
    int index = defaultSinksToUse.indexOf(currentDefaultSink);
    ++index;
    if(defaultSinksToUse.size() <= index ) {
        index = 0;
    }
    if(defaultSinksToUse.size() > 0) {
        currentDefaultSink = defaultSinksToUse.at(index);
        sinksManager->setDefaultSink(currentDefaultSink);
        saveDefaultSink();
    }    
    for(int i=0;i<itemCount;++i) {
        PulseAudioSinkListWidgetItem* sinkItem = dynamic_cast<PulseAudioSinkListWidgetItem*>(ui->listWidget->item(i));
        if(!sinkItem) {
            continue;
        }
        if(sinkItem->getSink().getName() == currentDefaultSink) {
            QFont font = sinkItem->font();
            font.setBold(true);
            sinkItem->setFont(font);
        } else {
            QFont font = sinkItem->font();
            font.setBold(false);
            sinkItem->setFont(font);
        }
    }
    sinkListLoadingComplete = true;
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
                    if(currentDefaultSink.isNull() || currentDefaultSink.isEmpty() ) {
                        currentDefaultSink = line;
                    }
                }
            }
        }
    } else {
        propFile.open(QIODevice::WriteOnly);
    }
    propFile.close();
}

void QAudioSwitcher::loadDefaultSink()
{
    QDir propDir(QDir::home().path()+QDir::separator()+".qaudioswitcher");
    QFile propFile(propDir.filePath("default_sink"));
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
                    currentDefaultSink = line;
                    QFont font = sinkItem->font();
                    font.setBold(true);
                    sinkItem->setFont(font);
                }
            }
        }
        propFile.close();
    }
}

void QAudioSwitcher::saveDefaultSink()
{
    QDir propDir(QDir::home().path()+QDir::separator()+".qaudioswitcher");
    QFile propFile(propDir.filePath("default_sink"));
    propFile.open(QIODevice::WriteOnly);
    QTextStream propFileStream(&propFile);
    propFileStream << currentDefaultSink << "\r\n";
    propFile.close();

}

