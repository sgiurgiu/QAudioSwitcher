#include "qaudioswitcher.h"
#include <QDesktopWidget>
#include <QMessageBox>
#include <QDebug>
#include <QTextStream>
#include <QDir>
#include <QFile>
#include <QScreen>

#include "ui_qaudioswitcher.h"
#include "audiomanagerfactory.h"
#include "audiomanager.h"

#include "devicelistwidgetitem.h"

namespace
{
    const QDir  propertiesDirectory(QDir::home().path()+QDir::separator()+".qaudioswitcher");
    const QString selectedSinksFilePath(propertiesDirectory.filePath("sinks"));
    const QString defaultSinkFilePath(propertiesDirectory.filePath("default_sink"));
}
QAudioSwitcher::QAudioSwitcher(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QAudioSwitcher),audioManager(AudioManagerFactory::CreateAudioManager(this)),
    sinkListLoadingComplete(false),currentDefaultSink("")
{
    ui->setupUi(this);
    ui->centralWidget->setLayout(ui->verticalLayout);

//    QObject::connect(ui->hideButton,SIGNAL(clicked()),ui->action_Hide, SLOT(trigger()));
    connect(ui->listWidget,SIGNAL(itemChanged(QListWidgetItem*)),this, SLOT(sinkSelectionChanged(QListWidgetItem*)));
    connect(ui->switchButton,SIGNAL(clicked()),this, SLOT(switchSink()));
    connect(audioManager, SIGNAL(signalError(const QString)),this, SLOT(showError(const QString)));
    connect(audioManager, SIGNAL(signalAddDevice(AudioDevice)),this, SLOT(addDevice(AudioDevice)));
    connect(audioManager, SIGNAL(signalSinkListComplete()),this, SLOT(sinkListComplete()));
    connect(ui->action_Quit,SIGNAL(triggered()),this,SLOT(quit()));
    auto screen = QGuiApplication::primaryScreen();
    if(screen)
    {
        this->move(screen->availableGeometry().center() - this->rect().center());
    }
    QDir::home().mkdir(".qaudioswitcher");
    setupTrayIcon();
    audioManager->retrieveSinks();
}

QAudioSwitcher::~QAudioSwitcher()
{
    delete ui;
    delete audioManager;
}

void QAudioSwitcher::setupTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(ui->action_Show);
    trayIconMenu->addAction(ui->action_Quit);
    trayIcon = new QSystemTrayIcon(this);
    QObject::connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
}
void QAudioSwitcher::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::DoubleClick) {
        if(this->isHidden()) {
            ui->action_Show->activate(QAction::Trigger);
        } else {
            ui->action_Hide->activate(QAction::Trigger);
        }
    }
}

void QAudioSwitcher::quit()
{
    QApplication::exit(0);
}

void QAudioSwitcher::showError(QString message)
{
    QMessageBox dialog;
    dialog.critical(this,"Error",message);
}

void QAudioSwitcher::addDevice(AudioDevice sink)
{    
    DeviceListWidgetItem* item=new DeviceListWidgetItem(std::move(sink),ui->listWidget);
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
    DeviceListWidgetItem* sinkItem = dynamic_cast<DeviceListWidgetItem*>(item);
    if(!sinkItem) {
        return;
    }
    qDebug()<< "Item selection changed "<<sinkItem->getDevice().getDescription()<<", with check state "<<sinkItem->checkState();
    saveSinksList();
}

void QAudioSwitcher::switchSink()
{
    sinkListLoadingComplete = false;
    //TODO: make it less lame. it stinks.
    int itemCount = ui->listWidget->count();
    QList<QString> defaultSinksToUse;
    for(int i=0;i<itemCount;++i) {
        DeviceListWidgetItem* sinkItem = dynamic_cast<DeviceListWidgetItem*>(ui->listWidget->item(i));
        if(!sinkItem) {
            continue;
        }
        if(sinkItem->checkState() == Qt::Checked) {
            defaultSinksToUse.append(sinkItem->getDevice().getName());
        }
    }
    int index = defaultSinksToUse.indexOf(currentDefaultSink);
    ++index;
    if(defaultSinksToUse.size() <= index ) {
        index = 0;
    }
    if(defaultSinksToUse.size() > 0) {
        currentDefaultSink = defaultSinksToUse.at(index);
        audioManager->setDefaultSink(currentDefaultSink);
        saveDefaultSink();
    }    
    for(int i=0;i<itemCount;++i) {
        DeviceListWidgetItem* sinkItem = dynamic_cast<DeviceListWidgetItem*>(ui->listWidget->item(i));
        if(!sinkItem) {
            continue;
        }
        if(sinkItem->getDevice().getName() == currentDefaultSink) {
            QFont font = sinkItem->font();
            font.setBold(true);
            sinkItem->setFont(font);
            if(QSystemTrayIcon::supportsMessages()) {
                trayIcon->showMessage("Output Changed",sinkItem->getDevice().getDescription());
            }            
            trayIcon->setIcon(sinkItem->icon());
            if(!trayIcon->isVisible()) {
                trayIcon->show();
            }
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
    QFile selectedSinksFile(selectedSinksFilePath);

    if(selectedSinksFile.open(QIODevice::WriteOnly))
    {
        QTextStream propFileStream(&selectedSinksFile);
        int itemCount = ui->listWidget->count();
        for(int i=0;i<itemCount; ++i) {
            DeviceListWidgetItem* sinkItem = dynamic_cast<DeviceListWidgetItem*>(ui->listWidget->item(i));
            if(!sinkItem) {
                continue;
            }
            if(sinkItem->checkState() == Qt::Checked) {
                propFileStream << sinkItem->getDevice().getName() << "\r\n";
            }
        }
    }
}

void QAudioSwitcher::loadSinksList()
{    
    QFile selectedSinksFile(selectedSinksFilePath);
    if(selectedSinksFile.exists() && selectedSinksFile.open(QIODevice::ReadOnly))
    {
        QTextStream propFileStream(&selectedSinksFile);
        QString line;
        while(!propFileStream.atEnd() && !(line = propFileStream.readLine()).isNull()) {
            int itemCount = ui->listWidget->count();
            for(int i=0;i<itemCount;++i) {
                DeviceListWidgetItem* sinkItem = dynamic_cast<DeviceListWidgetItem*>(ui->listWidget->item(i));
                if(!sinkItem) {
                    continue;
                }
                if(sinkItem->getDevice().getName() == line) {
                    sinkItem->setCheckState(Qt::Checked);
                    if(currentDefaultSink.isNull() || currentDefaultSink.isEmpty() ) {
                        currentDefaultSink = line;
                    }
                }
            }
        }
    } else {
        selectedSinksFile.open(QIODevice::WriteOnly);
        show();
    }
}

void QAudioSwitcher::loadDefaultSink()
{
    QFile defaultSinkFile(defaultSinkFilePath);

    if(defaultSinkFile.exists() && defaultSinkFile.open(QIODevice::ReadOnly))
    {
        QTextStream propFileStream(&defaultSinkFile);
        QString line;
        while(!propFileStream.atEnd() && !(line = propFileStream.readLine()).isNull()) {
            int itemCount = ui->listWidget->count();
            for(int i=0;i<itemCount;++i) {
                DeviceListWidgetItem* sinkItem = dynamic_cast<DeviceListWidgetItem*>(ui->listWidget->item(i));
                if(!sinkItem) {
                    continue;
                }
                if(sinkItem->getDevice().getName() == line) {
                    currentDefaultSink = line;
                    QFont font = sinkItem->font();
                    font.setBold(true);
                    sinkItem->setFont(font);
                    trayIcon->setIcon(sinkItem->icon());
                    if(!trayIcon->isVisible()) {
                        trayIcon->show();
                    }
                    audioManager->setDefaultSink(currentDefaultSink);
                }
            }
        }
    }
}

void QAudioSwitcher::saveDefaultSink()
{
    QFile defaultSinkFile(defaultSinkFilePath);
    if(defaultSinkFile.open(QIODevice::WriteOnly))
    {
        QTextStream propFileStream(&defaultSinkFile);
        propFileStream << currentDefaultSink << "\r\n";
    }
}

