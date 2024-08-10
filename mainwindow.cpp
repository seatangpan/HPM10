#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTimer>
#include <QMessageBox>
#include <QCheckBox>
#include <QRegularExpressionValidator>
#include <QDomDocument>
#include "TranslateLang.h"
#include "ProgressDisplayWidget.h"
#include "CheckBoxHeaderView.h"
#include "usblisten.h"

int MainWindow::EXIT_CODE_REBOOT = -10;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    hpm10 = new Commhpm10();
    isConnected = false;

    QPointer<UsbListen> usblisten = new UsbListen(this);
    qApp->installNativeEventFilter(usblisten.data());

    QSettings settings("HPM10.ini", QSettings::IniFormat, this);
    quint32 language = settings.value("language/index", "0").toInt();
    autoInterval = settings.value("autotest/time", "5000").toInt();
    audioInterval = settings.value("audiotest/time", "1000").toInt();
    isDebugTrace = settings.value("debug/value", "false").toBool();
    versionInterval = settings.value("version/time" , "5000").toInt();
    QString strVersion = settings.value("version/value", "HPM10_TOOL_V1.0").toString();
    savePath = settings.value("path/value", ".").toString();
    selectedTabIndex = settings.value("tab/value", "0").toInt();
    if(!strVersion.isEmpty())
    {
        this->setWindowTitle(strVersion);
    }

    TranslateLang::GetInst()->Load(language);

    QObject::connect(usblisten, SIGNAL(DevicePlugIn()), this, SLOT(onSerialChanged()));
    QObject::connect(usblisten, SIGNAL(DevicePlugOut()), this, SLOT(onSerialChanged()));
    QObject::connect(this, SIGNAL(signalOutputLog(QString)), ui->textEdit, SLOT(append(QString)));
    QObject::connect(this, SIGNAL(signalOutputCmd(QString)), ui->textEdit, SLOT(append(QString)));
    QObject::connect(this, SIGNAL(signalClearLog()), ui->textEdit, SLOT(clear()));

    QObject::connect(this, SIGNAL(signalAudioStatus(quint8)), this, SLOT(onAudioStatus(quint8)));
    QObject::connect(this, SIGNAL(signalAudioFinish()), this, SLOT(onAudioFinish()));
    QObject::connect(this, SIGNAL(signalCallFinish()), this, SLOT(onCallFinish()));
    QObject::connect(this, SIGNAL(signalResultSaveFinish()), this, SLOT(onResultSaveFinish()));
    QObject::connect(this, SIGNAL(signalCommand()), this, SLOT(onCommand()));
    QObject::connect(this, SIGNAL(sigalUpdateDataSend()), this, SLOT(onUpdateSend()));
    QObject::connect(this , SIGNAL(signalLostCommunication(void)) , this , SLOT(onSerialDisconnectedResp()));
    QObject::connect(this , SIGNAL(MainWindow::signalReboot()) , this , SLOT(MainWindow::onReboot()));

    QObject::connect(hpm10, SIGNAL(signalSerialDisconnected(void )), this, SLOT(onSerialDisconnectedResp()));
    QObject::connect(hpm10, SIGNAL(signalMsgRead(QByteArray)), this, SLOT(onMsgRead(QByteArray)));

    init();

    printBuildTime();
}

MainWindow::~MainWindow()
{
    delete hpm10;
    delete timerAutoTest;
    delete timerAudioStatus;
    delete timerVersion;
    delete timerConnect;
    delete ui;
}

void MainWindow::printBuildTime()
{
    QString dateTime = "HPM10 Tool Build At ";
    dateTime += __DATE__;
    dateTime += __TIME__;
    qInfo() << dateTime;
}

void MainWindow::initVersion()
{
    QMap<quint8,QString> mapVersionItems;
    for(quint32 index = 1; index <= 11; index++)
    {
        mapVersionItems.insert((quint8)index,GET_LAN(index));
    }

    ui->tableVersion->setColumnCount(4);
    ui->tableVersion->setRowCount((mapVersionItems.size() + 1) / 2);

    QMap<quint8,QString>::const_iterator iter = mapVersionItems.begin();
    quint8  t = 0;
    while(iter != mapVersionItems.end())
    {
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(iter.value());
        ui->tableVersion->setItem(t / 2, 0 + 2 * (t % 2), item);
        iter++;
        t++;
    }

    //设置等宽
    ui->tableVersion->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableVersion->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableVersion->horizontalHeader()->setVisible(false); //隐藏行表头
    ui->tableVersion->verticalHeader()->setVisible(false);   //隐藏列表头
    ui->tableVersion->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);//垂直滚动条按项移动
    
}

void MainWindow::updateVersion()
{
    QMap<quint8,QString> mapVersionItems;
    for(quint32 index = 1; index <= 11; index++)
    {
        mapVersionItems.insert((quint8)index,GET_LAN(index));
    }

    QMap<quint8,QString>::const_iterator iter = mapVersionItems.begin();
    quint8  t = 0;
    while(iter != mapVersionItems.end())
    {
        QMap<quint8,QString>::iterator it = mapVersion.find(iter.key());
        if(it != mapVersion.end())
        {
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText(it.value());
            ui->tableVersion->setItem(t / 2, 1 + 2 * (t % 2), item);
        }
        iter++;
        t++;
    }
}

void MainWindow::initInfo()
{
    QMap<quint8,QString> mapInfoItems;
    for(quint32 index = 1; index <= 8; index++)
    {
        mapInfoItems.insert((quint8)index,GET_LAN(index + 0x10));
    }

    QStringList listHead{GET_LAN(0x0000003C)};
    listHead << GET_LAN(0x0000007E);
    listHead << GET_LAN(0x0000007F);

    ui->tableInfo->setColumnCount(listHead.size());
    ui->tableInfo->setRowCount(mapInfoItems.size());
    ui->tableInfo->setHorizontalHeaderLabels(listHead);

    CheckBoxHeaderView *header = new CheckBoxHeaderView(0,Qt::Horizontal, this);
    ui->tableInfo->setHorizontalHeader(header);

    connect(header, &CheckBoxHeaderView::signalCheckStateChanged, [=](bool state)
    {
        int rowCnt = ui->tableInfo->rowCount();
        for(int t = 0; t < rowCnt; t++)
        {
            auto *checkBox = (QCheckBox *)ui->tableInfo->cellWidget(t, 0);
            checkBox->setChecked(state);
        }
    });

    connect(header, &CheckBoxHeaderView::signalEnabledChanged, [=](bool state)
    {
        int rowCnt = ui->tableInfo->rowCount();
        for(int t = 0; t < rowCnt; t++)
        {
            auto *checkBox = ui->tableInfo->cellWidget(t, 0);
            checkBox->setEnabled(state);
        }
    });

    QMap<quint8,QString>::const_iterator iter = mapInfoItems.begin();
    quint8  t = 0;
    while(iter != mapInfoItems.end())
    {
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(iter.value());
        ui->tableInfo->setItem(t, 1, item);

        QCheckBox *checkBox = new QCheckBox();
        checkBox->setText(tr(""));
        checkBox->setChecked(true);
        ui->tableInfo->setCellWidget(t, 0, checkBox);
        iter++;
        t++;
    }

    //设置等宽
    ui->tableInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableInfo->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableInfo->verticalHeader()->setVisible(false);   //隐藏列表头

    ui->tableInfo->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);//垂直滚动条按项移动

    QLineEdit *editVin = new QLineEdit();
    QObject::connect(editVin, &QLineEdit::textChanged, this, [=](QString value){
        editVin->setText(value.toUpper());
    });
    editVin->setMaxLength(17);
    editVin->setClearButtonEnabled(true);
    editVin->setValidator(new QRegularExpressionValidator(QRegularExpression(R"([a-hj-npr-zA-HJ-NPR-Z\d]*)"), editVin));
    ui->tableInfo->setCellWidget(0,2,editVin);

    QLineEdit *editColor = new QLineEdit();
    editColor->setMaxLength(32);
    editColor->setClearButtonEnabled(true);
    editColor->setValidator(new QRegularExpressionValidator(QRegularExpression(R"([\w\s\-#]*)"), editColor));
    ui->tableInfo->setCellWidget(1,2,editColor);

    //车辆类型
    QStringList TypeList
    {
        tr("NA"),tr("PassengerVehicleClassM1"),tr("BusesAndCoachesClassM2"),
        tr("BusesAndCoachesClassM3"),tr("LightCommercialVehiclesClassN1"),tr("HeavyDutyVehiclesClassN2"),
        tr("HeavyDutyVehiclesClassN3"),tr("MotorcyclesClassL1e"),tr("MotorcyclesClassL2e"),
        tr("MotorcyclesClassL3e"),tr("MotorcyclesClassL4e"),tr("MotorcyclesClassL5e"),
        tr("MotorcyclesClassL6e"),tr("MotorcyclesClassL7e")
    };
    QComboBox *comBoxVehicleType = new QComboBox();
    comBoxVehicleType->addItems(TypeList);
    ui->tableInfo->setCellWidget(2,2,comBoxVehicleType);

    //燃油类型
    QStringList TypeFuel
    {
        tr("NA"),tr("AsolineTankPresent"),tr("DieselTankPresent"),tr("CompressedNaturalGas"),
        tr("LiquidPropaneGas"),tr("ElectricEnergyStorage"),tr("HydrogenStorage"),tr("OtherStorage")
    };

    QComboBox *comFuel = new QComboBox();
    comFuel->addItems(TypeFuel);
    ui->tableInfo->setCellWidget(3,2,comFuel);

    //乘客数量
    QSpinBox *spBoxPassager = new QSpinBox();
    spBoxPassager->setRange(0,100);
    spBoxPassager->setSingleStep(1);
    spBoxPassager->setValue(0);
    ui->tableInfo->setCellWidget(4,2,spBoxPassager);

    //转向角
    QSpinBox *spBoxAngle = new QSpinBox();
    spBoxAngle->setRange(0,180);
    spBoxAngle->setSingleStep(1);
    spBoxAngle->setValue(0);
    ui->tableInfo->setCellWidget(5,2,spBoxAngle);

    //3D初始位校准
    QStringList _3DCheckList{tr("NA"),GET_LAN(0x0000000C),GET_LAN(0x0000000D)};
    QComboBox *comBox3D = new QComboBox();
    comBox3D->addItems(_3DCheckList);
    ui->tableInfo->setCellWidget(6,2,comBox3D);

    //六轴传感器
    QStringList _6SensorList{tr("NA"),GET_LAN(0x0000000E),GET_LAN(0x0000000F)};
    QComboBox *comBox6Sensor = new QComboBox();
    comBox6Sensor->addItems(_6SensorList);
    ui->tableInfo->setCellWidget(7,2,comBox6Sensor);
}

void MainWindow::updateInfo()
{
    QMap<quint8,QString> mapInfoItems;
    for(quint32 index = 1; index <= 8; index++)
    {
        mapInfoItems.insert((quint8)index,GET_LAN(index + 0x10));
    }

    QMap<quint8,QString>::const_iterator iter = mapInfoItems.begin();
    quint8  t = 0;
    while(iter != mapInfoItems.end())
    {
        QMap<quint8,QString>::iterator it = mapInfo.find(iter.key());
        if(it != mapInfo.end())
        {
            if(1  == it.key())
            {
                QLineEdit *editVin = (QLineEdit *)ui->tableInfo->cellWidget(t, 2);
                editVin->setText(it.value());
            }
            else if(2  == it.key())
            {
                QLineEdit *editColor = (QLineEdit *)ui->tableInfo->cellWidget(t, 2);
                editColor->setText(it.value());
            }
            else if((3  == it.key())||(7  == it.key())|| (8  == it.key()))
            {
                QComboBox *comBox = (QComboBox *)ui->tableInfo->cellWidget(t, 2);
                quint8 curIndex = it.value().toInt();
                if(3 == it.key())
                {
                    if(curIndex >= comBox->count())
                    {
                        comBox->setCurrentIndex(0);
                    }
                    else
                    {
                        comBox->setCurrentIndex(curIndex);
                    }

                }
                else
                {
                    if((curIndex + 1) >= comBox->count())
                    {
                        comBox->setCurrentIndex(0);
                    }
                    else
                    {
                        comBox->setCurrentIndex(curIndex);
                    }
                    comBox->setCurrentIndex(curIndex + 1);
                }

            }
            else if(4 == it.key())
            {
                QComboBox *comBox = (QComboBox *)ui->tableInfo->cellWidget(t, 2);
                quint32 fuel = it.value().toInt();
                if(0x01 & fuel){comBox->setCurrentIndex(1);}
                else if(0x02 & fuel){comBox->setCurrentIndex(2);}
                else if(0x04 & fuel){comBox->setCurrentIndex(3);}
                else if(0x08 & fuel){comBox->setCurrentIndex(4);}
                else if(0x10 & fuel){comBox->setCurrentIndex(5);}
                else if(0x20 & fuel){comBox->setCurrentIndex(6);}
                else if(0x40 & fuel){comBox->setCurrentIndex(7);}
                else{comBox->setCurrentIndex(0);}
            }
            else if((5  == it.key())|| (6  == it.key()))
            {
                //乘客数量
                QSpinBox *spBox = (QSpinBox *)ui->tableInfo->cellWidget(t, 2);
                quint8 value = it.value().toInt();
                spBox->setValue(value);
            }
        }
        iter++;
        t++;
    }
}

void MainWindow::initTest()
{
    QMap<quint8,QString> mapTestItems;
    for(quint32 index = 1; index <= 0x0C; index++)
    {
        mapTestItems.insert((quint8)index,GET_LAN(index + 0x20));
    }

    QStringList listHead{GET_LAN(0x0000003C)};

    for(quint32 index = 0x19; index <= 0x1C; index++)
    {
        listHead << GET_LAN(index);
    }

    ui->tableTest->setColumnCount(listHead.size());
    ui->tableTest->setRowCount(mapTestItems.size());
    ui->tableTest->setHorizontalHeaderLabels(listHead);

    CheckBoxHeaderView *header = new CheckBoxHeaderView(0,Qt::Horizontal, this);
    ui->tableTest->setHorizontalHeader(header);

    connect(header, &CheckBoxHeaderView::signalCheckStateChanged, [=](bool state)
    {
        int rowCnt = ui->tableTest->rowCount();
        for(int t = 0; t < rowCnt; t++)
        {
            QCheckBox *checkBox = (QCheckBox *)ui->tableTest->cellWidget(t, 0);
            checkBox->setChecked(state);
        }
    });

    connect(header, &CheckBoxHeaderView::signalEnabledChanged, [=](bool state)
    {
        int rowCnt = ui->tableTest->rowCount();
        for(int t = 0; t < rowCnt; t++)
        {
            QCheckBox *checkBox = (QCheckBox *)ui->tableTest->cellWidget(t, 0);
            checkBox->setEnabled(state);
        }
    });

    QMap<quint8,QString>::const_iterator iter = mapTestItems.begin();
    quint8  t = 0;
    while(iter != mapTestItems.end())
    {
        QCheckBox *checkBox = new QCheckBox();
        checkBox->setText(tr(""));
        if(t != mapTestItems.size() - 1)
        {
            checkBox->setChecked(true);
        }
        ui->tableTest->setCellWidget(t, 0, checkBox);

        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(iter.value());
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));
        ui->tableTest->setItem(t, 1, item);

        QTableWidgetItem* item2 = new QTableWidgetItem;
        item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
        ui->tableTest->setItem(t , 2 , item2);

        QTableWidgetItem* item3 = new QTableWidgetItem;
        if (item3)
        {
            item3->setBackgroundColor(QColor(Qt::GlobalColor::white));
            item3->setFlags(item3->flags() & (~Qt::ItemIsEditable));
            ui->tableTest->setItem(t , 3 , item3);
        }

        QTableWidgetItem* item4 = new QTableWidgetItem;
        if (item4)
        {
            item4->setBackgroundColor(QColor(Qt::GlobalColor::white));
            item4->setFlags(item4->flags() & (~Qt::ItemIsEditable));
            ui->tableTest->setItem(t , 4 , item4);
        }

        iter++;
        t++;
    }

    //设置等宽
    ui->tableTest->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableTest->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableTest->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableTest->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableTest->verticalHeader()->setVisible(false);   //隐藏列表头

    ui->tableTest->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);//垂直滚动条按项移动
}

void MainWindow::updateTest()
{
    QMap<quint8,QString> mapTestItems;
    for(quint32 index = 1; index <= 0x0C; index++)
    {
        mapTestItems.insert((quint8)index,GET_LAN(index + 0x20));
    }

    QMap<quint8,QString>::const_iterator iter = mapTestItems.begin();
    quint8  t = 0;
    while(iter != mapTestItems.end())
    {
        QMap<quint8,QList<quint16>>::iterator it = mapSelftest.find(iter.key());

        QList<quint16> result{0,0,0};
        if(it != mapSelftest.end())
        {
            result = it.value();
        }

        for(qint16 i = 0; i < result.size(); i++)
        {
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText(QString::number(result[i]));

            if (1 == i)
            {
                if ((result[1] == result[0]) && (0 != result[1]))
                {
                    item->setBackgroundColor(QColor(Qt::GlobalColor::green));
                }
            }
            else if (2 == i)
            {
                if (0 != result[i])
                {
                    item->setBackgroundColor(QColor(Qt::GlobalColor::red));
                }
            }  
            ui->tableTest->setItem(t, i + 2, item);
        }
        iter++;
        t++;
    }
}

void MainWindow::initCommand(const QString &filename)
{
    QList<QStringList> items;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        file.close();
        return ;
    }

    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        file.close();
        return;
    }

    QDomElement project = doc.documentElement();
    QDomNode node = project.firstChild();
    while(!node.isNull())
    {
        if(node.isElement())
        {
            QDomElement element = node.toElement();
            if(!element.isNull())
            {
                if(element.nodeName() == QString("command"))
                {
                    QString strCommand = element.attribute("value");
                    QString strDelay = element.attribute("delay");
                    if(!strCommand.isEmpty())
                    {
                        items.append(QStringList() << strCommand << strDelay);
                    }
                }
            }
        }
        node = node.nextSibling();
    }
    file.close();

    if(items.isEmpty())
    {
        QMessageBox::information(this, GET_LAN(0x0000005E),
                                 GET_LAN(0x0000007B), QMessageBox::Ok);
        return;
    }

    ui->tableCommand->clear();
    QStringList listHead{GET_LAN(0x0000003C)};
    for(quint32 index = 0x54; index <= 0x55; index++)
    {
        listHead << GET_LAN(index);
    }

    listHead << tr("");

    ui->tableCommand->setColumnCount(listHead.size());
    ui->tableCommand->setRowCount(items.size());
    ui->tableCommand->setHorizontalHeaderLabels(listHead);
    CheckBoxHeaderView *header = new CheckBoxHeaderView(0,Qt::Horizontal, this);
    ui->tableCommand->setHorizontalHeader(header);

    connect(header, &CheckBoxHeaderView::signalCheckStateChanged, [=](bool state)
    {
        int rowCnt = ui->tableCommand->rowCount();
        for(int t = 0; t < rowCnt; t++)
        {
            QCheckBox *checkBox = (QCheckBox *)ui->tableCommand->cellWidget(t, 0);
            checkBox->setChecked(state);
        }
    });

    connect(header, &CheckBoxHeaderView::signalEnabledChanged, [=](bool state)
    {
        int rowCnt = ui->tableCommand->rowCount();
        for(int t = 0; t < rowCnt; t++)
        {
            QCheckBox *checkBox = (QCheckBox *)ui->tableCommand->cellWidget(t, 0);
            checkBox->setEnabled(state);
        }
    });


    for(quint16 t = 0; t < items.size(); t++)
    {
        QCheckBox *checkBox = new QCheckBox();
        checkBox->setText(tr(""));
        checkBox->setChecked(true);
        ui->tableCommand->setCellWidget(t, 0, checkBox);

        for(quint16 i = 0; i < items[t].size(); i++)
        {
            QLineEdit *edit = new QLineEdit();
            if(0 == i)
            {
                edit->setValidator(new QRegularExpressionValidator(QRegularExpression(R"([\w\s\+\?=]*)"), edit));
                QObject::connect(edit, &QLineEdit::textChanged, this, [=](QString value){
                    edit->setText(value.toUpper());
                });
                QObject::connect(edit, SIGNAL(returnPressed()), this, SLOT(onSendCommand()));
            }
            else if(i > 0)
            {
                edit->setValidator(new QRegularExpressionValidator(QRegularExpression(R"([\d]{0,6})"), edit));
            }
            edit->setClearButtonEnabled(true);
            edit->setText(items[t][i]);
            ui->tableCommand->setCellWidget(t, i + 1, edit);
        }
        QPushButton *btn = new QPushButton();
        btn->setText(GET_LAN(0x00000074));
        ui->tableCommand->setCellWidget(t, 3, btn);
        QObject::connect(btn, SIGNAL(pressed()), this, SLOT(onSendCommand()));
    }

    //设置等宽
    ui->tableCommand->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableCommand->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableCommand->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tableCommand->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->tableCommand->verticalHeader()->setVisible(false);   //隐藏列表头
    ui->tableCommand->horizontalHeader()->setVisible(true);
    ui->tableCommand->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);//垂直滚动条按项移动
}

void MainWindow::appendMsgout(QString msg)
{
    if(msg.contains("Communication"))
    {
        msg = "<font color=\"#000000\">" + msg + "</font>";
        if(isDebugTrace)
        {
            emit signalOutputLog(msg);
        }
    }
    else if(msg.contains("Info"))
    {
        msg = "<font color=\"#0000FF\">" + msg + "</font>";
        emit signalOutputLog(msg);
    }
    else if(msg.contains("Warning"))
    {
        msg = "<font color=\"#FF00FF\">" + msg + "</font>";
        emit signalOutputLog(msg);
    }
    else if(msg.contains("Critical"))
    {
        msg = "<font color=\"#FF0000\">" + msg + "</font>";
        emit signalOutputLog(msg);
    }
    else if(msg.contains("Error"))
    {
        msg = "<font color=\"#FF0000\">" + msg + "</font>";
        emit signalOutputLog(msg);
    }
    else
    {
        msg = "<font color=\"#000000\">" + msg + "</font>";
    }
}

void MainWindow::appendMsgcmd(QString msg, quint16 msgtype)
{
    if(quint16(cmdlist::CMD_REQ) == msgtype)
    {
        msg = "<font color=\"#0000FF\">" + msg + "</font>";
    }
    else if(quint16(cmdlist::CMD_ANS) == msgtype)
    {
        msg = "<font color=\"#FF00FF\">" + msg + "</font>";
    }
    else
    {
        msg = "<font color=\"#0000FF\">" + msg + "</font>";
    }
    emit signalOutputCmd(msg);
}

void MainWindow::enableDebug(bool bTrue)
{
    isDebugTrace = bTrue;
    if(bTrue)
    {
        appendMsgcmd(tr("debug = on"), quint16(cmdlist::CMD_ANS));
        QSettings settings("HPM10.ini", QSettings::IniFormat, this);
        //保存配置
        settings.beginGroup("debug");
        settings.setValue("value", "true");
        settings.endGroup();
    }
    else
    {
        appendMsgcmd(tr("debug = off"), quint16(cmdlist::CMD_ANS));
        QSettings settings("HPM10.ini", QSettings::IniFormat, this);
        //保存配置
        settings.beginGroup("debug");
        settings.setValue("value", "false");
        settings.endGroup();
    }
}

void MainWindow::resetTestUi()
{
    for (int t = 0; t < ui->tableTest->rowCount(); t++)
    {
        QTableWidgetItem* item = ui->tableTest->item(t , 2);
        if (item)
        {
            item->setText("");
        }

        item = ui->tableTest->item(t , 3);
        if (item)
        {
            item->setBackgroundColor(QColor(Qt::GlobalColor::white));
            item->setText("");
        }

        item = ui->tableTest->item(t , 4);
        if (item)
        {
            item->setBackgroundColor(QColor(Qt::GlobalColor::white));
            item->setText("");
        }
    }
}

void MainWindow::onSerialChanged()
{
    quint32 index = 0;
    QList<QSerialPortInfo> listProtInfo = hpm10->getCommNames();
    ui->cbx_comm_name->clear();
    qSort(listProtInfo.begin() , listProtInfo.end() , \
        [](const QSerialPortInfo& a , const QSerialPortInfo& b) { return a.portName() < b.portName(); });
    for (int i = 0; i < listProtInfo.size(); i++)
    {
        QString portName = QString("%1(%2)").arg(listProtInfo[i].description()).arg(listProtInfo[i].portName());
        ui->cbx_comm_name->addItem(portName);
        if (listProtInfo[i].description().contains("ASR Serial Port")
                || listProtInfo[i].description().contains("Neoway Serial Port"))
        {
            index = i;
        }
    }
    ui->cbx_comm_name->setCurrentIndex(index);
}

void MainWindow::onMsgRead(QByteArray buffer)
{
    if(buffer.size() < 10)
    {
        return;
    }
    //按功能码解析
    quint16 code = (buffer[7] << 8) | buffer[8];
    switch ((code - 0x8000))
    {
    case (quint16)Commhpm10::paramlist::HP10_CONNECT:
        if(buffer.size() > 10)
        {
            onConnectResp(buffer[11]);
        }
        break;
    case (quint16)Commhpm10::paramlist::HP10_DISCONNECT:
        if(buffer.size() > 10)
        {
            onDisConnectResp(buffer[11]);
        }
        break;
    case (quint16)Commhpm10::paramlist::HP10_READ_VERSION:
        if(buffer.size() > 11)
        {
            QByteArray resp = QByteArray(buffer.data() + 12, buffer.size() - 12);
            onGetVersionResp(buffer[11], resp);
        }
        break;
    case (quint16)Commhpm10::paramlist::HP10_WRITE_VERSION:
        if(buffer.size() > 11)
        {
        }
        break;
    case (quint16)Commhpm10::paramlist::HP10_READ_INFO:
        if(buffer.size() > 11)
        {
            QByteArray resp = QByteArray(buffer.data() + 12, buffer.size() - 12);
            onGetInfoResp(buffer[11], resp);
        }
        break;
    case (quint16)Commhpm10::paramlist::HP10_WRIET_INFO:
        if(buffer.size() > 11)
        {
            QByteArray resp = QByteArray(buffer.data() + 12, buffer.size() - 12);
            onSetInfoResp(buffer[11], resp);
        }
        break;
    case (quint16)Commhpm10::paramlist::HP10_UPDATE:
        {
            QByteArray resp = QByteArray(buffer.data() + 12, buffer.size() - 12);
            onUpdateResp(buffer[11], resp);
        }
        break;
    case (quint16)Commhpm10::paramlist::HP10_SELFCHECK:
        if(buffer.size() > 11)
        {
            QByteArray resp = QByteArray(buffer.data() + 12, buffer.size() - 13);
            onSelftestResp(buffer[11], resp);
        }
        break;
    case (quint16)Commhpm10::paramlist::HP10_VOICE_TEST:
        if(buffer.size() > 11)
        {
            QByteArray resp = QByteArray(buffer.data() + 12, buffer.size() - 13);
            onAudioResp(buffer[11], resp);
        }
        break;
    case (quint16)Commhpm10::paramlist::HP10_REBOOT:

        break;
    case (quint16)Commhpm10::paramlist::HP10_TESTMODE:
        if(buffer.size() > 12)
        {
            quint16 seq = buffer[9] * 0x100 + buffer[10];
            onCommandResp(seq , QByteArray(buffer.data() + 11, buffer.size() - 12));
        }
        break;
    case (quint16)Commhpm10::paramlist::HP10_CALLTEST:
        if (buffer.size() > 11)
        {
            QByteArray resp = QByteArray(buffer.data() + 12 , buffer.size() - 13);
            onCallTest(buffer[11] , resp);
        }
        break;
    default:
        break;
    }
}

void MainWindow::onConnectResp(quint8 result)
{
    timerConnect->stop();
    if(0x00 == result)
    {
        ui->btn_comm_open->setText(GET_LAN(0x0000001F));
        ui->cbx_comm_name->setEnabled(false);
        ui->cbx_comm_baudrate->setEnabled(false);
        qDebug() << GET_LAN(0x0000004D);
        enableUI(true);
        isConnected = true;
        qWarning()<< GET_LAN(0x00000020);

        resetTestUi();
        ui->progressBar->setValue(0);
    }
    else if(0x01 == result)
    {
        qWarning()<< GET_LAN(0x0000002D);
    }
    else if(0x02 == result)
    {
        qWarning()<< GET_LAN(0x0000002E);
        ui->btn_comm_open->setText(GET_LAN(0x0000001F));
        ui->cbx_comm_name->setEnabled(false);
        ui->cbx_comm_baudrate->setEnabled(false);
        enableUI(true);
        isConnected = true;
    }
    else
    {
        qWarning()<< GET_LAN(0x0000002F);
    }

    if(0x02 == result || 0x00 == result)
    {
        const quint8 version[] = {0x0B,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B};
        hpm10->getVersion(QByteArray((char *)version,sizeof(version)));

        const quint8 info[] = {0x08,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
        hpm10->getInfo(QByteArray((char *)info,sizeof(info)));

        timerVersion->start(versionInterval);
    }
}

void MainWindow::onDisConnectResp(quint8 result)
{
    if(0x00 == result)
    {
        qWarning()<< GET_LAN(0x00000030);
        onSerialDisconnectedResp();
    }
    else if(0x01 == result)
    {
        if(timerVersion)
        {
            timerVersion->stop();
        }
        qWarning() << GET_LAN(0x0000002D);
    }
    else if(0x02 == result)
    {
        qWarning()<< GET_LAN(0x00000031);
        onSerialDisconnectedResp();
    }
    else
    {
        if(timerVersion)
        {
            timerVersion->stop();
        }
        qWarning()<< GET_LAN(0x00000057);
    }
}

void MainWindow::init()
{
    QStringList listBuadrate = hpm10->getCommBps();
    ui->cbx_comm_baudrate->clear();
    ui->cbx_comm_baudrate->addItems(listBuadrate);
    if(listBuadrate.size())
    {
        ui->cbx_comm_baudrate->setCurrentIndex(listBuadrate.size() - 1);
    }

    onSerialChanged();

    initVersion();
    initInfo();
    initTest();
    initCommand("Cmdconfig.xml");

    ui->label->setText(GET_LAN(0x00000033));
    ui->label_2->setText(GET_LAN(0x00000034));
    ui->btn_comm_open->setText(GET_LAN(0x0000001E));
    ui->btn_open_update_file->setText(GET_LAN(0x00000036));
    ui->btn_update->setText(GET_LAN(0x00000037));
    ui->groupBox->setTitle(GET_LAN(0x00000047));
    ui->groupBox_2->setTitle(GET_LAN(0x00000084));
    ui->label_7->setText(GET_LAN(0x00000038));

    ui->tabWidget->setTabText(0, GET_LAN(0x00000039));
    ui->tabWidget->setTabText(1, GET_LAN(0x0000003A));
    ui->tabWidget->setTabText(2, GET_LAN(0x0000003B));
    ui->tabWidget->setTabText(3, GET_LAN(0x00000035));

    ui->label_3->setText(GET_LAN(0x0000003E));
    ui->btn_test_start->setText(GET_LAN(0x0000003F));
    ui->btn_test_end->setText(GET_LAN(0x00000040));
    ui->btn_test_suspend->setText(GET_LAN(0x00000066));
    ui->btn_save_test_result->setText(GET_LAN(0x0000006F));

    ui->btn_audio_start->setText(GET_LAN(0x00000048));
    ui->btn_audio_end->setText(GET_LAN(0x00000049));
    ui->btn_audio_result->setText(GET_LAN(0x0000004A));

    ui->btn_info_read->setText(GET_LAN(0x00000041));
    ui->btn_info_write->setText(GET_LAN(0x00000042));
    ui->btn_info_save->setText(GET_LAN(0x00000043));

    ui->btn_command_start->setText(GET_LAN(0x0000003F));
    ui->btn_command_end->setText(GET_LAN(0x00000040));
    ui->btn_command_load->setText(GET_LAN(0x0000006B));
    ui->btn_command_save->setText(GET_LAN(0x0000007C));
    ui->btn_command_clear->setText(GET_LAN(0x0000006E));
    ui->label_5->setText(GET_LAN(0x00000055));
    ui->label_6->setText(GET_LAN(0x00000056));

    ui->btn_log_save->setText(GET_LAN(0x0000004B));
    ui->btn_log_clear->setText(GET_LAN(0x0000004C));

    ui->btn_call_start->setText(GET_LAN(0x00000048));
    ui->btn_call_end->setText(GET_LAN(0x00000049));

    ui->btn_save_path_select->setText(GET_LAN(0x0000006D));

    if(!isConnected)
    {
        enableUI(false);
        enableTabWidget(0xFFFF);
        enableHeadView(true);
    }

    QStringList listProcess;
    for(quint32 index = 0x00000058; index <= 0x0000005D; index++)
    {
        listProcess << GET_LAN(index);
    }
    ui->widget->setProcessList(listProcess);

    ui->ledit_save_path->setText(QDir(savePath).absolutePath());

    ui->textEdit->setReadOnly(true);

    ui->tabWidget->setCurrentIndex(selectedTabIndex);

    isReboot = false;
    ui->label_4->setText(GET_LAN(0x00000070));
    ui->cbx_language->clear();
    ui->cbx_language->addItem(GET_LAN(0x00000071));
    ui->cbx_language->addItem(GET_LAN(0x00000072));
    ui->cbx_language->setCurrentIndex(TranslateLang::GetInst()->GetCurType());
    isReboot = true;

    ui->textEdit->clear();
}

void MainWindow::onReboot()
{
    qWarning()<<"reboot";
    qApp->exit(EXIT_CODE_REBOOT);
}

void MainWindow::enableUI(bool bTrue)
{
    ui->btn_audio_start->setEnabled(bTrue);
    ui->btn_call_start->setEnabled(bTrue);
    ui->btn_audio_end->setEnabled(false);
    ui->btn_call_end->setEnabled(false);
    ui->btn_audio_result->setEnabled(false);
    ui->btn_test_start->setEnabled(bTrue);
    if(bTrue)
    {
       ui->btn_test_end->setEnabled(isAutotestStart);
       ui->btn_test_suspend->setEnabled(isAutotestStart);
       ui->btn_test_end->setEnabled(isAutotestStart);
       ui->btn_update->setEnabled(!ui->ledit_updatefile->text().isEmpty());
    }
    else
    {
        ui->btn_test_end->setEnabled(false);
        ui->btn_test_suspend->setEnabled(false);
        ui->btn_test_end->setEnabled(false);
        ui->btn_update->setEnabled(false);
    }

    ui->btn_info_read->setEnabled(bTrue);
    ui->btn_info_write->setEnabled(bTrue);
    ui->btn_info_save->setEnabled(bTrue);

    ui->btn_command_start->setEnabled(bTrue);
    ui->btn_command_end->setEnabled(false);
    ui->btn_command_load->setEnabled(bTrue);
    ui->btn_command_save->setEnabled(bTrue);
    ui->btn_command_clear->setEnabled(bTrue);
    enableCommandButtons(bTrue);
    enableHeadView(bTrue);
    ui->btn_open_update_file->setEnabled(bTrue);
}

void MainWindow::enableCommandButtons(bool bTrue)
{
    int rowCnt = ui->tableCommand->rowCount();
    int colCnt = ui->tableCommand->rowCount();
    for(int t = 0; t < rowCnt; t++)
    {
        if(colCnt > 3)
        {
            QPushButton * btn = (QPushButton *)ui->tableCommand->cellWidget(t, 3);
            if(btn)
            {
                btn->setEnabled(bTrue);
            }
        }
    }
}

void MainWindow::enableHeadView(bool bTrue)
{
    CheckBoxHeaderView * headTest = (CheckBoxHeaderView *)ui->tableTest->horizontalHeader();
    headTest->setEnabledStatus(bTrue);

    CheckBoxHeaderView * headInfo = (CheckBoxHeaderView *)ui->tableInfo->horizontalHeader();
    headInfo->setEnabledStatus(bTrue);

    CheckBoxHeaderView * headCommand = (CheckBoxHeaderView *)ui->tableCommand->horizontalHeader();
    headCommand->setEnabledStatus(bTrue);
}

void MainWindow::enableTabWidget(const quint16 &index)
{
    quint16 tabCnt = ui->tabWidget->count();
    if(index <= tabCnt)
    {
        for(quint16 t = 0; t < tabCnt; t++)
        {
            if(t == index)
            {
                ui->tabWidget->setTabEnabled(t, true);
            }
            else
            {
                ui->tabWidget->setTabEnabled(t , false);
            }
        }
    }
    else if(index == 0xFFFF)
    {
        for(quint16 t = 0; t < tabCnt; t++)
        {
            ui->tabWidget->setTabEnabled(t, true);
        }
    }
}


void MainWindow::on_btn_comm_open_clicked()
{
    versionFrameCount = 0;

    if(false == isConnected)
    {
        QString sCommName = ui->cbx_comm_name->currentText();
        QList<QSerialPortInfo> listProtInfo = hpm10->getCommNames();
        foreach (const auto &info,listProtInfo)
        {
            if (sCommName.contains(info.portName()))
            {
                sCommName = info.portName();
            }
        }
        qint32 nBuadrate = hpm10->getCommBps(ui->cbx_comm_baudrate->currentText());
        bool bReuslt = hpm10->open(sCommName, nBuadrate);
        if (!bReuslt)
        {
            qWarning()<< GET_LAN(0x0000001D);
            hpm10->close();
            QMessageBox::information(this, GET_LAN(0x0000005E),
                                     GET_LAN(0x0000001D), QMessageBox::Ok);
            enableUI(false);
        }
        else
        {
            hpm10->connectReq();

            if (timerConnect != nullptr)
            {
                delete timerConnect;
            }

            timerConnect = new QTimer(this);
            connect(timerConnect, &QTimer::timeout, this, [=](){
                if(GET_LAN(0x0000001E) == ui->btn_comm_open->text())
                {
                    QMessageBox::information(this , GET_LAN(0x0000005E) ,
                        GET_LAN(0x00000077) , QMessageBox::Ok);
                }
            });
            timerConnect->setSingleShot(true);
            timerConnect->start(2000);

            const quint8 version[] = {0x0B,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B};
            hpm10->getVersion(QByteArray((char *)version,sizeof(version)));

            if (timerVersion != nullptr)
            {
                delete timerVersion;
            }

            timerVersion = new QTimer;
            QObject::connect(timerVersion, &QTimer::timeout,this,[=](){
                if(hpm10->isOpen())
                {
                    hpm10->getVersion(QByteArray((char *)version,sizeof(version)));
                    versionFrameCount++;
                    if (versionFrameCount > 3)
                    {
                        qDebug() << "comm disconnected...";
                        emit signalLostCommunication();
                    }
                }
            });
        }
    }
    else
    {
        hpm10->disconnectReq();

        QEventLoop loop;
        QTimer::singleShot(500,&loop,SLOT(quit()));
        loop.exec();

        if(isConnected)
        {
            onDisConnectResp(0);
        }
    }
}

void MainWindow::on_btn_open_update_file_clicked()
{
    QString s = QFileDialog::getOpenFileName(this,GET_LAN(0x0000005F),".",
                                             GET_LAN(0x00000078));
    if(!s.isEmpty())
    {
        ui->ledit_updatefile->setText(s);
        ui->btn_update->setEnabled(true);
    }
}

void MainWindow::on_btn_update_clicked()
{
    QString filename = ui->ledit_updatefile->text();
    if(filename.isEmpty())
    {
        QMessageBox::information(this, GET_LAN(0x0000005E),
                                 GET_LAN(0x00000079), QMessageBox::Ok);
        return;
    }

    QFileInfo fileInfo(filename);
    if((!fileInfo.isFile()) || (0 == fileInfo.size()))
    {
        QMessageBox::information(this, GET_LAN(0x0000005E),
                                 GET_LAN(0x00000079), QMessageBox::Ok);
        return;
    }

    QByteArray updateStart(11, 0x00);
    const quint32 framesize = 1024;
    const quint32 filesize = fileInfo.size();
    const quint16 framecnt = (filesize + framesize - 1) / framesize;
    updateStart[0] = 0x01;
    updateStart[1] = (filesize >> 24) & 0xFF;
    updateStart[2] = (filesize >> 16) & 0xFF;
    updateStart[3] = (filesize >> 8) & 0xFF;
    updateStart[4] = (filesize >> 0) & 0xFF;
    updateStart[5] = (framesize >> 24) & 0xFF;
    updateStart[6] = (framesize >> 16) & 0xFF;
    updateStart[7] = (framesize >> 8) & 0xFF;
    updateStart[8] = (framesize >> 0) & 0xFF;
    updateStart[9] = (framecnt >> 8) & 0xFF;
    updateStart[10] = (framecnt >> 0) & 0xFF;

    listUpdateFrame.clear();

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        file.close();
        return ;
    }

    QByteArray buffer = file.readAll();
    file.close();
    if(buffer.isEmpty())
    {
        QMessageBox::information(this, GET_LAN(0x0000005E),
                                 GET_LAN(0x00000079), QMessageBox::Ok);
        return;
    }

    quint32 pos = 0;
    quint16 frameindex = 1;

    while (pos < filesize)
    {
        QByteArray updateData(3, 0x02);
        updateData[1] = (frameindex >> 8) & 0xFF;
        updateData[2] = (frameindex >> 0) & 0xFF;
        updateData.append(buffer.data() + pos, std::min(framesize, filesize - pos));
        listUpdateFrame.push_back(updateData);
        pos += framesize;
        frameindex++;
    }
    QByteArray md5 = QCryptographicHash::hash(buffer,QCryptographicHash::Md5);
    QByteArray updateStop(1, 0x03);
    updateStop.append(md5);
    listUpdateFrame.push_back(updateStop);

    updateFrameCount = listUpdateFrame.size();

    if(timerVersion)
    {
        timerVersion->stop();
        enableTabWidget(3);
    }

    hpm10->update(updateStart);
}

void MainWindow::onGetVersionResp(quint8 result, QByteArray buffer)
{
    mapVersion.clear();
    quint16 len = buffer.size();
    quint16 i = 0;
    quint8 cnt = 0;

    versionFrameCount = 0;

    while (i < len)
    {
        quint8 param = buffer[i];
        switch (param)
        {
        case 0x01:{
                QString strValue = QString(QByteArray(buffer.data() + i + 1, 64));
                mapVersion.insert(param,strValue);
                i += 65;
            }
            break;
        case 0x02:{
                QString strValue = QString(QByteArray(buffer.data() + i + 1, 20));
                mapVersion.insert(param,strValue);
                i += 21;
            }
            break;
        case 0x03:{
                QString strValue = QString(QByteArray(buffer.data() + i + 1, 15));
                mapVersion.insert(param,strValue);
                i += 16;
            }
            break;
        case 0x04:{
                QString strValue = QString(QByteArray(buffer.data() + i + 1, 64));
                mapVersion.insert(param,strValue);
                i += 65;
            }
            break;
        case 0x05:{
                QString strValue = QString(QByteArray(buffer.data() + i + 1, 16));
                mapVersion.insert(param,strValue);
                i += 17;
            }
            break;
        case 0x06:{
                QString strValue = QString(QByteArray(buffer.data() + i + 1, 32));
                mapVersion.insert(param,strValue);
                i += 33;
            }
            break;
        case 0x07:{
                QString strValue = QString(QByteArray(buffer.data() + i + 1, 32));
                mapVersion.insert(param,strValue);
                i += 33;
            }
            break;
        case 0x08:{
                float voltage = (buffer[i + 1] * 0x100 + buffer[i + 2]) / 1000.0;
                mapVersion.insert(param,QString::number(voltage) + tr("V"));
                i += 3;
            }
            break;
        case 0x09:{
                if(0x00 == buffer[i + 1])
                {
                    mapVersion.insert(param,GET_LAN(0x0000004E));
                }
                else if(0x01 == buffer[i + 1])
                {
                    mapVersion.insert(param,GET_LAN(0x0000004F));
                }
                i += 2;
            }
            break;
        case 0x0A:{
                QStringList list
                {
                    tr("Off mode"),tr("Standby mode"),tr("ERA mode"),tr("Emergency Call mode"),
                    tr("Test mode"),tr("Service Station mode"),tr("Software Downloading mode")
                };
                if(int(buffer[i + 1]) < list.size())
                {
                    mapVersion.insert(param,list.at(buffer[i + 1]));
                }
                i += 2;
            }
            break;
        case 0x0B:{
                if(0x00 == buffer[i + 1])
                {
                    mapVersion.insert(param,GET_LAN(0x00000050));
                }
                else if(0x01 == buffer[i + 1])
                {
                    mapVersion.insert(param,GET_LAN(0x00000051));
                }
                i += 2;
            }
            break;
        default:
            i += len;
            break;
        }

        cnt++;

        if(cnt >= result )
        {
            break;
        }
    }
    updateVersion();
}

void MainWindow::onGetInfoResp(quint8 result, QByteArray buffer)
{
    mapInfo.clear();
    quint16 len = buffer.size();
    quint16 i = 0;
    quint8 cnt = 0;

    while (i < len)
    {
        quint8 param = buffer[i];
        switch (param)
        {
        case 0x01:{
                QString strValue = QString(QByteArray(buffer.data() + i + 1, 17));
                mapInfo.insert(param,strValue);
                i += 18;
            }
            break;
        case 0x02:{
                QString strValue = QString(QByteArray(buffer.data() + i + 1, 32));
                mapInfo.insert(param,strValue);
                i += 33;
            }
            break;
        case 0x03:{
                QString strValue = QString::number(buffer[i+1]);
                mapInfo.insert(param,strValue);
                i += 2;
            }
            break;
        case 0x04:{
                quint32 fuel = (buffer[i+1] << 24) + (buffer[i+2] << 16) + (buffer[i+3] << 8) + buffer[i+4];
                QString strValue = QString::number(fuel);
                mapInfo.insert(param,strValue);
                i += 5;
            }
            break;
        case 0x05:{
                QString strValue = QString::number(buffer[i+1]);
                mapInfo.insert(param,strValue);
                i += 2;
            }
            break;
        case 0x06:{
                QString strValue = QString::number(buffer[i+1]);
                mapInfo.insert(param,strValue);
                i += 2;
            }
            break;
        case 0x07:{
                QString strValue = QString::number(buffer[i+1]);
                mapInfo.insert(param,strValue);
                i += 2;
            }
            break;
        case 0x08:{
                QString strValue = QString::number(buffer[i+1]);
                mapInfo.insert(param,strValue);
                i += 2;
            }
            break;
        default:
            i += len;
            break;
        }
        cnt++;
        if(cnt >= result )
        {
            break;
        }
    }
    updateInfo();

    qWarning() << GET_LAN(0x0000007A);
}

void MainWindow::onSetInfoResp(quint8 result, QByteArray buffer)
{
    QMap<quint8,QString> mapInfoItems;
    for(quint32 index = 1; index <= 8; index++)
    {
        mapInfoItems.insert((quint8)index,GET_LAN(index + 0x10));
    }

    quint16 len = buffer.size();
    quint16 i = 0;
    qint8 cnt = result;

    QStringList setResult;

    while (i < len)
    {
        QMap<quint8,QString>::iterator it = mapInfoItems.find(buffer[i]);
        if(it != mapInfoItems.end())
        {

            if(0 == buffer[i+1])
            {
                setResult << it.value() + tr(":OK");
            }
            else if(1 == buffer[i+1])
            {
                setResult << it.value() + tr(":NG");
            }
        }
        cnt--;
        i += 2;
        if(cnt <= 0)
        {
            break;
        }
    }

    if(!setResult.isEmpty())
    {
        foreach (auto var, setResult)
        {
            qWarning() << var;
        }
    }
}

void MainWindow::onSelftestResp(quint8 result, QByteArray buffer)
{
    if(0 == result)
    {
        return;
    }
    testResult = buffer;
    quint16 len = buffer.size();
    quint16 i = 0;
    quint8 cnt = 0;

    while (i < len)
    {
        quint8 param = buffer[i];
        quint8 value = buffer[i + 1];

        QMap<quint8,QList<quint16>>::iterator it = mapSelftest.find(param);
        if(it != mapSelftest.end())
        {
            QList<quint16> &resultcnt = it.value();
            resultcnt[0] += 1;
            if(0x00 == value)
            {
                //test ok
                resultcnt[1] += 1;
            }
            else if(0x01 == value)
            {
                //test fail
                resultcnt[2] += 1;
            }
        }
        else
        {
            QList<quint16> resultcnt{0, 0, 0};
            resultcnt[0] += 1;
            if(0x00 == value)
            {
                //test ok
                resultcnt[1] += 1;
            }
            else if(0x01 == value)
            {
                //test fail
                resultcnt[2] += 1;
            }
            mapSelftest.insert(param, resultcnt);
        }
        cnt++;
        i += 2;
        if(cnt >= result )
        {
            break;
        }
    }

    if(isUpdateTestUi)
    {
        updateTest();
    }
}

void MainWindow::onAudioResp(quint8 result, QByteArray buffer)
{
    if(0x01 == result)  //测试开始
    {
        if(0x00 != buffer[0])
        {
            qWarning() << GET_LAN(0x00000060);
            return;
        }

        if(timerVersion)
        {
            timerVersion->stop();
        }

        ui->widget->setCurrentStep(0);

        if(timerAudioStatus)
        {
            timerAudioStatus->start(audioInterval);
        }
        ui->btn_audio_start->setEnabled(false);
        ui->btn_call_start->setEnabled(false);
        ui->btn_audio_end->setEnabled(true);
        ui->btn_test_start->setEnabled(false);
        enableTabWidget(0);
        enableHeadView(false);
    }
    else if(0x02 == result) //测试完成
    {
        if(0x00 == buffer[0])
        {
            isUpdateTestUi = false;
            testResult.clear();

            QByteArray testItem(3, 0x00);
            quint8 cnt = 0;
            int rowCnt = ui->tableTest->rowCount();
            for(int t = 0; t < rowCnt; t++)
            {
                QCheckBox *checkBox = (QCheckBox *)ui->tableTest->cellWidget(t, 0);
                if(Qt::Checked == checkBox->checkState())
                {
                    testItem.append(t + 1);
                    cnt++;
                }
            }
            testItem[2] = cnt;
            hpm10->selfTest(testItem);

            ui->widget->setCurrentStep(6);
            ui->btn_audio_end->setEnabled(false);
            ui->btn_call_end->setEnabled(false);
            ui->btn_audio_start->setEnabled(true);
            ui->btn_call_start->setEnabled(true);
            ui->btn_test_start->setEnabled(true);
            enableTabWidget(0xFFFF);
            enableHeadView(true);
            if(timerVersion)
            {
                timerVersion->start();
            }
        }
    }
    else if(0x03 == result) //测试停止
    {
        if(0x00 == buffer[0])
        {
            ui->widget->setCurrentStep(0);
            ui->btn_audio_end->setEnabled(false);
            ui->btn_call_end->setEnabled(false);
            ui->btn_audio_start->setEnabled(true);
            ui->btn_call_start->setEnabled(true);
            ui->btn_test_start->setEnabled(true);
            enableTabWidget(0xFFFF);
            enableHeadView(true);
            if(timerVersion)
            {
                timerVersion->start();
            }
        }
    }
    else if(0x04 == result) //测试状态查询
    {
        quint8 status = buffer[0];
        qWarning() << GET_LAN(0x00000061) << status;
        if(0 == status)
        {
            if(timerAudioStatus)
            {
                timerAudioStatus->stop();
            }
        }
        if(5 == status)
        {
            ui->widget->setCurrentStep(status);
            if(timerAudioStatus)
            {
                timerAudioStatus->stop();
                emit signalAudioFinish();
            }
        }
        else
        {
            ui->widget->setCurrentStep(status);
        }
    }
}

void MainWindow::onCommandResp(quint16 result, QByteArray buffer)
{
    QString strAns = QString("Ans = '%1' seq = '%2'").arg(QString(buffer)).arg(result);
    appendMsgcmd(strAns, quint16(cmdlist::CMD_ANS));
}

void MainWindow::onUpdateResp(quint16 result, QByteArray buffer)
{
    if(0x01 == result)
    {
        if(0x00 == buffer[0])
        {
            emit sigalUpdateDataSend();
        }
        else
        {
            emit sigalUpdateDataSend();
            qWarning() << GET_LAN(0x00000081);
        }
        ui->btn_update->setEnabled(false);
    }
    else if(0x02 == result)
    {
        if(0x00 == buffer[0])
        {
            emit sigalUpdateDataSend();
        }
        else
        {
            emit sigalUpdateDataSend();
            qWarning() << GET_LAN(0x00000082);;
        }
    }
    else if(0x03 == result)
    {
        if(0x00 == buffer[0])
        {
            ui->progressBar->setValue(100);
            qWarning() << GET_LAN(0x00000083);;
        }
        else
        {
            ui->progressBar->setValue(100);
            qWarning() << GET_LAN(0x00000081);;
        }
        ui->btn_update->setEnabled(true);
        if(timerVersion)
        {
            timerVersion->start();
            enableTabWidget(0xFFFF);
        }
    }
}

void MainWindow::onCallTest(quint8 result , QByteArray buffer)
{
    if (0x01 == result)  //测试开始
    {
        if (0x00 != buffer[0])
        {
            callResult = 0x01;
            qWarning() << GET_LAN(0x00000085);
            return;
        }

        if (timerVersion)
        {
            timerVersion->stop();
        }

        ui->btn_call_end->setEnabled(true);
        ui->btn_audio_start->setEnabled(false);
        ui->btn_call_start->setEnabled(false);
        ui->btn_test_start->setEnabled(false);
        enableTabWidget(0);
        enableHeadView(false);
    }
    else if (0x02 == result) //测试完成
    {
        if (0x00 != buffer[0])
        {
            qWarning() << GET_LAN(0x00000085);
            callResult = 0x01;
            return;
        }

        if (timerVersion)
        {
            timerVersion->start();
        }

        ui->btn_audio_end->setEnabled(false);
        ui->btn_call_end->setEnabled(false);
        ui->btn_audio_start->setEnabled(true);
        ui->btn_call_start->setEnabled(true);
        ui->btn_test_start->setEnabled(true);
        ui->btn_audio_result->setEnabled(true);
        enableTabWidget(0xFFFF);
        enableHeadView(true);
    }
}

void MainWindow::onSerialDisconnectedResp()
{
    if(timerAutoTest)
    {
        QObject::disconnect(timerAutoTest, 0, this, 0);//解除信号槽
    }
    if(timerAudioStatus)
    {
        QByteArray audioItem(1, 0x03);
        hpm10->audioTest(audioItem);
        QObject::disconnect(timerAudioStatus, 0, this, 0);//解除信号槽
    }
    if(timerVersion)
    {
        QObject::disconnect(timerVersion, 0, this, 0);//解除信号槽
    }

    resetTestUi();
    ui->progressBar->setValue(0);

    callResult = 0xFF;

    mutexOpen.lock();
    isAutotestStart = false;

    hpm10->close();
    isConnected = false;
    mutexOpen.unlock();
    ui->btn_comm_open->setText(GET_LAN(0x0000001E));
    ui->cbx_comm_name->setEnabled(true);
    ui->cbx_comm_baudrate->setEnabled(true);
    enableUI(false);
    enableTabWidget(0xFFFF);
}

void MainWindow::on_btn_info_read_clicked()
{
    QByteArray infoItem;
    quint8 len = 0x00;
    infoItem.append(len);
    int rowCnt = ui->tableInfo->rowCount();
    for(int t = 0; t < rowCnt; t++)
    {
        QCheckBox *checkBox = (QCheckBox *)ui->tableInfo->cellWidget(t, 0);
        if(Qt::Checked == checkBox->checkState())
        {
            infoItem.append(t + 1);
        }
    }

    if(infoItem.size() > 1)
    {
        infoItem[0] = infoItem.size() - 1;
        hpm10->getInfo(infoItem);
    }
    else
    {
        qWarning() << GET_LAN(0x00000075);
        return;
    }
}

void MainWindow::on_btn_info_write_clicked()
{
    QMap<quint8,QString> mapInfoItems;
    for(quint32 index = 1; index <= 8; index++)
    {
        mapInfoItems.insert((quint8)index,GET_LAN(index + 0x10));
    }

    QByteArray infoItem;
    quint8 len = 0x00;
    quint8 cnt = 0;
    infoItem.append(len);
    int rowCnt = ui->tableInfo->rowCount();
    for(int t = 0; t < rowCnt; t++)
    {
        QCheckBox *checkBox = (QCheckBox *)ui->tableInfo->cellWidget(t, 0);
        if(Qt::Checked == checkBox->checkState())
        {
            if(0 == t)
            {
                QLineEdit *editVin = (QLineEdit *)ui->tableInfo->cellWidget(t, 2);
                QString strContent =editVin->text();
                if(strContent.size() >= 17)
                {
                    infoItem.append(t + 1);
                    QByteArray byteContent = strContent.toLocal8Bit();
                    char buffer[17] = {0x00};
                    memcpy_s(buffer, sizeof(buffer), byteContent.data(), byteContent.size());
                    infoItem.append(buffer, sizeof(buffer));
                    cnt++;
                }
                else
                {
                    QString strInfo = GET_LAN(0x00000080) + ":"
                            + GET_LAN(0x00000011);
                    qWarning() << strInfo;
                    return;
                }
            }
            else if(1 == t)
            {
                QLineEdit *editColor = (QLineEdit *)ui->tableInfo->cellWidget(t, 2);
                QString strContent = editColor->text();
                if(!strContent.isEmpty())
                {
                    infoItem.append(t + 1);
                    QByteArray byteContent = strContent.toLocal8Bit();
                    char buffer[32] = {0x00};
                    memcpy_s(buffer, sizeof(buffer), byteContent.data(), byteContent.size());
                    infoItem.append(buffer, sizeof(buffer));
                    cnt++;
                }
            }
            else if((2 == t) || (6 == t) || (7 == t))
            {
                QComboBox *comBox = (QComboBox *)ui->tableInfo->cellWidget(t, 2);
                qint16 index = comBox->currentIndex();
                if(index > 0)
                {
                    infoItem.append(t + 1);
                    if(2 == t)
                    {
                        infoItem.append(index);
                    }
                    else
                    {
                        infoItem.append(index - 1);
                    }
                    cnt++;
                }
            }
            else if((4 == t) || (5 == t))
            {
                QSpinBox *spBox = (QSpinBox *)ui->tableInfo->cellWidget(t, 2);
                infoItem.append(t + 1);
                infoItem.append((quint8)spBox->value());
                cnt++;
            }
            else if(3 == t)
            {
                QByteArray byteContent(4,0x00);
                QComboBox *comBox = (QComboBox *)ui->tableInfo->cellWidget(t, 2);
                qint16 index = comBox->currentIndex();
                if(index > 0)
                {
                    byteContent[3] = pow(2, comBox->currentIndex() - 1);
                    infoItem.append(t + 1);
                    infoItem.append(byteContent);
                    cnt++;
                }
            }
        }
    }

    if(0 == cnt)
    {
        qWarning() << GET_LAN(0x00000075);
        return;
    }
    else
    {
        if(infoItem.size() > 1)
        {
            infoItem[0] = cnt;
            hpm10->setInfo(infoItem);
        }
    }
}

void MainWindow::on_btn_log_clear_clicked()
{
    emit signalClearLog();
}

void MainWindow::on_btn_log_save_clicked()
{
    QString logFileName = QFileDialog::getSaveFileName(this,GET_LAN(0x00000063),".",
                                                       GET_LAN(0x00000064));
    if(!logFileName.isEmpty())
    {
        //写入文件
        QFile file(logFileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
        {
            file.close();
            return ;
        }
        QTextStream stream(&file);
        stream << ui->textEdit->toPlainText() << "\r\n";;
        file.flush();
        file.close();
    }
}

void MainWindow::on_btn_test_start_clicked()
{
    //每次执行开始测试，就清除掉测试的结果
    mapSelftest.clear();
    listSelfTestFrame.clear();
    isUpdateTestUi = true;

    QByteArray infoItem(3, 0x00);
    quint8 cnt = 0;
    int rowCnt = ui->tableTest->rowCount();
    for(int t = 0; t < rowCnt; t++)
    {
        QCheckBox *checkBox = (QCheckBox *)ui->tableTest->cellWidget(t, 0);
        if(Qt::Checked == checkBox->checkState())
        {
            infoItem.append(t + 1);
            cnt++;
        }

        resetTestUi();
    }
    infoItem[2] = cnt;

    if(0 == cnt)
    {
        qWarning() << GET_LAN(0x00000075);
        return;
    }

    for(quint32 i = 0; i < (quint32)ui->spinBox->text().toInt(); i++)
    {
        listSelfTestFrame.append(infoItem);
    }

    if(listSelfTestFrame.size())
    {
        auto header = (CheckBoxHeaderView*)ui->tableTest->horizontalHeader();
        emit header->setEnabledStatus(false);

        isAutotestStart = true;
        ui->btn_test_start->setEnabled(false);
        ui->btn_test_suspend->setEnabled(true);
        ui->btn_test_end->setEnabled(true);
        ui->btn_save_test_result->setEnabled(false);
        ui->btn_audio_start->setEnabled(false);
        ui->btn_call_start->setEnabled(false);
        enableHeadView(false);
        enableTabWidget(0);

        onSelfTest();

        if (timerAutoTest != nullptr)
        {
            delete timerAutoTest;
        }

        timerAutoTest = new QTimer;
        QObject::connect(timerAutoTest, SIGNAL(timeout()), this, SLOT(onSelfTest()));
        timerVersion->stop();
        timerAutoTest->start(autoInterval);
    }
    else
    {
        qWarning() << GET_LAN(0x00000076);
        return;
    }
}

void MainWindow::on_btn_test_suspend_clicked()
{
    if(isAutotestStart)
    {
        isAutotestStart = false;
        ui->btn_test_suspend->setText(GET_LAN(0x00000065));
        if(timerAutoTest)
        {
            timerAutoTest->stop();
        }
    }
    else
    {
        isAutotestStart = true;
        ui->btn_test_suspend->setText(GET_LAN(0x00000066));
        if(timerAutoTest)
        {
            timerAutoTest->start();
        }
    }
}

void MainWindow::on_btn_test_end_clicked()
{
    if(timerAutoTest)
    {
        timerAutoTest->stop();
    }

    QEventLoop loop;
    QTimer::singleShot(500,&loop,SLOT(quit()));
    loop.exec();

    ui->btn_test_suspend->setText(GET_LAN(0x00000066));
    isAutotestStart = false;
    ui->btn_audio_start->setEnabled(true);
    ui->btn_call_start->setEnabled(true);
    ui->btn_test_suspend->setEnabled(false);
    ui->btn_test_start->setEnabled(true);
    ui->btn_test_end->setEnabled(false);
    ui->btn_save_test_result->setEnabled(true);
    enableHeadView(true);
    enableTabWidget(0xFFFF);

    QByteArray infoItem(3, 0x00);
    quint8 cnt = 0;
    int rowCnt = ui->tableTest->rowCount();
    for(int t = 0; t < rowCnt; t++)
    {
        QCheckBox *checkBox = (QCheckBox *)ui->tableTest->cellWidget(t, 0);
        if(Qt::Checked == checkBox->checkState())
        {
            infoItem.append(t + 1);
            cnt++;
        }
        checkBox->setEnabled(true);
    }

    infoItem[0] = 0x01; //停止测试
    infoItem[2] = cnt;
    if(infoItem.size() > 3)
    {
        hpm10->selfTest(infoItem);
    }
    timerVersion->start();
}


void MainWindow::on_btn_audio_start_clicked()
{
    QByteArray audioItem(1,0x01);
    bool ret = hpm10->audioTest(audioItem);
    if(!ret)
    {
        qWarning() << GET_LAN(0x00000060);
    }

    if (timerAudioStatus != nullptr)
    {
        delete timerAudioStatus;
    }

    timerAudioStatus = new QTimer;
    QObject::connect(timerAudioStatus, &QTimer::timeout,this,[=](){
        if(hpm10->isOpen())
        {
            QByteArray statusItem(1,0x04);
            hpm10->audioTest(statusItem);
        }
    });
}

void MainWindow::onAudioStatus(quint8 status)
{
    if(0 == status)
    {
        if(timerAudioStatus)
        {
            timerAudioStatus->stop();
        }
    }
    if(5 == status)
    {
        qWarning() << GET_LAN(0x00000061) << status;
        ui->widget->setCurrentStep(status);
        if(timerAudioStatus)
        {
            timerAudioStatus->stop();
            emit signalAudioFinish();
        }
    }
    else
    {
        qWarning() << GET_LAN(0x00000061) << status;
        ui->widget->setCurrentStep(status);
    }
}

void MainWindow::onAudioFinish()
{
    QByteArray audioItem(2,0x02);
    QMessageBox msgBox;
    msgBox.setText(GET_LAN(0x00000067));
    msgBox.setWindowTitle(GET_LAN(0x00000047));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.button(QMessageBox::Yes)->setText(GET_LAN(0x00000068));
    msgBox.button(QMessageBox::No)->setText(GET_LAN(0x00000069));
    int msg = msgBox.exec();
    if(QMessageBox::Yes == msg)
    {
        audioResult = 0x00;
        audioItem[1] = 0x00;
    }
    else if(QMessageBox::No == msg)
    {
        audioResult = 0x01;
        audioItem[1] = 0x01;
    }
    hpm10->audioTest(audioItem);
}

void MainWindow::onCallFinish()
{
    QByteArray callItem(2 , 0x02);
    QMessageBox msgBox;
    msgBox.setText(GET_LAN(0x00000086));
    msgBox.setWindowTitle(GET_LAN(0x00000084));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    QPushButton* btnYes = (QPushButton*)msgBox.button(QMessageBox::Yes);
    QPushButton* btnNo = (QPushButton*)msgBox.button(QMessageBox::No);

    btnYes->setText(GET_LAN(0x00000068));
    btnNo->setText(GET_LAN(0x00000069));

    QPalette palette = btnYes->palette();
    palette.setColor(QPalette::Button , QColor(Qt::GlobalColor::green));
    btnYes->setPalette(palette);
    btnYes->setAutoFillBackground(true);
    btnYes->setFlat(true);

    palette = btnNo->palette();
    palette.setColor(QPalette::Button , QColor(Qt::GlobalColor::red));
    btnNo->setPalette(palette);
    btnNo->setAutoFillBackground(true);
    btnNo->setFlat(true);

    int msg = msgBox.exec();
    if (QMessageBox::Yes == msg)
    {
        callResult = 0x00;
        callItem[1] = 0x00;
    }
    else if (QMessageBox::No == msg)
    {
        callResult = 0x01;
        callItem[1] = 0x01;
    }
    hpm10->calltest(callItem);
}

void MainWindow::onResultSaveFinish()
{
    ui->btn_audio_result->setEnabled(false);
    audioResult = 0xFF;
    callResult = 0xFF;
}

void MainWindow::onCommand()
{
    ui->btn_command_clear->setEnabled(false);
    ui->btn_command_start->setEnabled(false);
    ui->btn_command_end->setEnabled(true);
    ui->btn_command_load->setEnabled(false);
    ui->btn_command_save->setEnabled(false);
    enableTabWidget(2);
    enableHeadView(false);

    bool isEmpty{true};

    for(quint32 i = 0; i < (quint32)ui->spCount->text().toInt(); i++)
    {
        foreach (auto var, listCommand)
        {
            QString strCommand = std::get<0>(var).remove(QRegExp("\\s")).toUpper();
            quint32 interval= std::get<1>(var);

            if(strCommand.contains("DEBUGON"))
            {
                enableDebug(true);
                continue;
            }
            else if(strCommand.contains("DEBUGOFF"))
            {
                enableDebug(false);
                continue;
            }

            isEmpty = false;
            QByteArray commandItem(1, strCommand.length());
            commandItem.append(strCommand.toLocal8Bit());
            QString strSeq = QString("Req = '%1' Seq = '%2'").arg(strCommand).arg(QString::number(hpm10->getSeqence()));
            appendMsgcmd(strSeq, quint16(cmdlist::CMD_REQ));
            hpm10->cmdTest(commandItem);

            if((!isExcuteCommand) || (!hpm10->isOpen()))
            {
                return;
            }

            QEventLoop loop;
            QTimer::singleShot(interval,&loop,SLOT(quit()));
            loop.exec();
        }

        if(isEmpty)
        {
            qWarning() << GET_LAN(0x00000076);
            break;
        }

        QEventLoop loop;
        QTimer::singleShot((quint32)ui->spTime->text().toInt(),&loop,SLOT(quit()));
        loop.exec();
    }

    ui->btn_command_clear->setEnabled(true);
    ui->btn_command_start->setEnabled(true);
    ui->btn_command_end->setEnabled(false);
    ui->btn_command_load->setEnabled(true);
    ui->btn_command_save->setEnabled(true);
    enableTabWidget(0xFFFF);
    enableHeadView(true);
    enableCommandButtons(true);

    for(int t = 0; t < ui->tableCommand->rowCount(); t++)
    {
        QCheckBox *checkBox = (QCheckBox *)ui->tableCommand->cellWidget(t, 0);
        checkBox->setEnabled(true);
    }

    if(timerVersion)
    {
        timerVersion->start();
    }
}

void MainWindow::onSendCommand()
{
    QModelIndex index;
    QPushButton *senderObj = qobject_cast<QPushButton*>(this->sender());
    if(nullptr != senderObj)
    {
        index = ui->tableCommand->indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));
    }
    else
    {
        QLineEdit *senderObj1 = qobject_cast<QLineEdit*>(this->sender());
        if(nullptr != senderObj1)
        {
            index = ui->tableCommand->indexAt(QPoint(senderObj1->frameGeometry().x(),senderObj1->frameGeometry().y()));
        }
        else
        {
            return;
        }
    }

    int rowCnt = ui->tableCommand->rowCount();

    for(int t = 0; t < rowCnt; t++)
    {
        if(index.row() == t)
        {
            QLineEdit *edit = (QLineEdit *)ui->tableCommand->cellWidget(t, 1);
            QString strCommand = edit->text().remove(QRegExp("\\s")).toUpper();
            if(!strCommand.isEmpty())
            {
                QByteArray commandItem(1, strCommand.length());
                if(strCommand.contains("DEBUGON"))
                {
                    enableDebug(true);
                    continue;
                }
                else if(strCommand.contains("DEBUGOFF"))
                {
                    enableDebug(false);
                    continue;
                }
                if(isConnected)
                {
                    commandItem.append(strCommand.toLocal8Bit());
                    QString strSeq = QString("Req = '%1' Seq = '%2'").arg(strCommand).arg(QString::number(hpm10->getSeqence()));
                    appendMsgcmd(strSeq, quint16(cmdlist::CMD_REQ));
                    hpm10->cmdTest(commandItem);
                }
            }
            else
            {
                appendMsgcmd(tr(""), quint16(cmdlist::CMD_REQ));
            }
        }
    }
}

void MainWindow::onUpdateSend()
{
    if(!listUpdateFrame.isEmpty())
    {
        float percent = 100 - (listUpdateFrame.size() * 100 + 1) / updateFrameCount;
        ui->progressBar->setValue(percent);
        hpm10->update(listUpdateFrame.front());
        listUpdateFrame.pop_front();
    }
}

void MainWindow::onSelfTest()
{
    if(!listSelfTestFrame.isEmpty())
    {
       if(isAutotestStart)
       {
           hpm10->selfTest(listSelfTestFrame.front());
           listSelfTestFrame.pop_front();
       }
    }

    if(listSelfTestFrame.isEmpty())
    {
        on_btn_test_end_clicked();
    }
}

void MainWindow::on_btn_audio_end_clicked()
{
    if(timerAudioStatus)
    {
        timerAudioStatus->stop();
    }

    QByteArray audioItem(1, 0x03);
    hpm10->audioTest(audioItem);
}

void MainWindow::on_btn_save_path_select_clicked()
{
    QString s = QFileDialog::getExistingDirectory(this,GET_LAN(0x0000006D),savePath);
    if(!s.isEmpty())
    {
        ui->ledit_save_path->setText(s);
        QSettings settings("HPM10.ini", QSettings::IniFormat, this);
        //保存配置
        settings.beginGroup("path");
        settings.setValue("value", s);
        settings.endGroup();
        savePath = s;
    }
}


void MainWindow::makeSaveAutoTest(QString &title, QString &content)
{
    QStringList listItem;
    QStringList listValue;
    listItem << GET_LAN(0x00000052);    //日期
    listItem << GET_LAN(0x00000053);    //时间
    listItem << GET_LAN(0x00000001);    //终端编号

    for(quint32 index = 1; index <= 0x0C; index++)
    {
        listItem << GET_LAN(index + 0x20);
    }

    listValue << QString("'%1,'%2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd")).arg(QDateTime::currentDateTime().toString("hh:mm:ss"));
    listValue << QString("'") + ui->tableVersion->item(0 , 1)->text();   //终端编号
    auto uRowCnt = ui->tableTest->rowCount();

    for(auto t = 0; t < uRowCnt; t++)
    {
        QString strTotal = ui->tableTest->item(t,2)->text();
        QString strSuccess = ui->tableTest->item(t,3)->text();
        listValue << (tr("'") + strSuccess + "/" + strTotal);
    }
    title = listItem.join(",");
    content = listValue.join(",");
}

void MainWindow::makeSaveTest(QString &title, QString &content)
{
    QStringList listItem;
    QStringList listValue;
    //listItem << GET_LAN(0x00000052);    //日期
    listItem << GET_LAN(0x00000053);    //时间

    for(quint32 index = 1; index <= 11; index++)
    {
       listItem << GET_LAN(index);
    }

    for(quint32 index = 1; index <= 0x0C; index++)
    {
        listItem << GET_LAN(index + 0x20);
    }

    listItem << GET_LAN(0x00000047);  //声音测试

    listValue << QString("'%1").arg(QDateTime::currentDateTime().toString("hh:mm:ss"));


    for(quint32 t = 0; t < 11; t++)
    {
        QString strContent = ui->tableVersion->item(t / 2 , 1 + 2 * (t % 2))->text();
        if (t < 3)
        {
            listValue << QString("'") + strContent;
        }
        else
        {
            listValue << strContent;
        }
    }

    QStringList listTest;
    for(qint32 t = 0; t < ui->tableTest->rowCount(); t++)
    {
        listTest << tr("");
    }

    for(quint16 t = 0; t < testResult.size(); t += 2)
    {
        quint8 param = testResult[t];
        quint8 value = testResult[t + 1];
        if((param < listTest.size()) && (param > 0))
        {
            if(0 == value)
            {
                listTest[param - 1] = tr("OK");
            }
            else
            {
                listTest[param - 1] = tr("NG");
            }
        }
    }

    listValue.append(listTest);

    if(0x00 == audioResult)
    {
        listValue << tr("OK");
    }
    else if(0x01 == audioResult)
    {
        listValue << tr("NG");
    }
    else
    {
        listValue << tr("");
    }

    title = listItem.join(",");
    content = listValue.join(",");
}

void MainWindow::on_btn_save_test_result_clicked()
{
    QString path = ui->ledit_save_path->text();
    if(path.isEmpty())
    {
        QMessageBox::information(this, GET_LAN(0x0000005E),
                                 GET_LAN(0x00000062), QMessageBox::Ok);
        return;
    }
    else
    {
        path += tr("/TESTDATA/AUTOTEST");
    }

    QString currentDate = QDateTime::currentDateTime().toString("yyyyMMdd");

    path += tr("/") + currentDate;

    dirExist(path);

    QEventLoop loop;
    QTimer::singleShot(500,&loop,SLOT(quit()));
    loop.exec();

    mutexSave.lock();

    QString strId = ui->tableVersion->item(0,1)->text();

    //创建csv文件
    QString currentTime = QDateTime::currentDateTime().toString("hhmmss");
    QString csvName = path + "/" + strId + currentTime + ".csv";

    QString strTitle;
    QString strContent;
    makeSaveAutoTest(strTitle, strContent);


    //写入文件
    QFile file(csvName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        file.close();
        return ;
    }
    QTextStream stream(&file);
    stream << strTitle << "\r\n";
    stream << strContent << "\r\n";
    file.flush();
    file.close();
    qWarning()<< GET_LAN(0x0000006A) + ":" + csvName;

    mutexSave.unlock();
}

void MainWindow::on_btn_info_save_clicked()
{
    QString path = ui->ledit_save_path->text();
    if(path.isEmpty())
    {
        QMessageBox::information(this, GET_LAN(0x0000005E),
                                 GET_LAN(0x00000062), QMessageBox::Ok);
        return;
    }
    else
    {
        path += tr("/TESTDATA/VEHICLE DATA");
    }

    dirExist(path);

    QEventLoop loop;
    QTimer::singleShot(500,&loop,SLOT(quit()));
    loop.exec();

    mutexSave.lock();

    //创建csv文件
    QString currentDate = QDateTime::currentDateTime().toString("yyyyMMdd");
    QString csvName = path + "/" +currentDate + ".csv";

    bool isFileExist = false;
    QFileInfo fileInfo(csvName);
    if(fileInfo.isFile())
    {
        isFileExist = true;
    }

    QString strTitle;
    QString strContent;
    makeSaveInfo(strTitle, strContent);

    //写入文件
    QFile file(csvName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        file.close();
        return ;
    }
    QTextStream stream(&file);
    if(isFileExist)
    {
        stream << strContent << "\r\n";
    }
    else
    {
        stream << strTitle << "\r\n";
        stream << strContent << "\r\n";
    }
    file.flush();
    file.close();

    qWarning()<< GET_LAN(0x0000006A)  + ":" + csvName;

    mutexSave.unlock();
}

bool MainWindow::dirExist(const QString &path)
{
    bool ret = true;
    QDir dir(path);
    if(dir.exists())
    {
        //存在当前文件夹
    }
    else
    {
        //不存在则创建
        ret = dir.mkpath(path);
    }
    return ret;
}

void MainWindow::makeSaveInfo(QString &title, QString &content)
{
    QStringList listItem;
    QStringList listValue;
    listItem << GET_LAN(0x00000052);    //日期
    listItem << GET_LAN(0x00000053);    //时间
    listItem << GET_LAN(0x00000001);    //终端编号
    listItem << GET_LAN(0x00000003);    //IMEI
    listItem << GET_LAN(0x00000002);    //ICCID
    listItem << GET_LAN(0x0000000A);    //运行模式

    for(quint32 index = 1; index <= 8; index++)
    {
        listItem << GET_LAN(index + 0x10);
    }

    listValue << QString("'%1,'%2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd")).arg(QDateTime::currentDateTime().toString("hh:mm:ss"));

    listValue << QString("'") + ui->tableVersion->item(0,1)->text();   //终端编号
    listValue << QString("'") + ui->tableVersion->item(1,1)->text();   //IMEI
    listValue << QString("'") + ui->tableVersion->item(0,3)->text();   //ICCID
    listValue << ui->tableVersion->item(4,3)->text();   //运行模式

    QLineEdit *editVin = (QLineEdit *)ui->tableInfo->cellWidget(0, 2);  //VIN
    QString strVin = editVin->text();
    listValue << strVin;

    QLineEdit* editColor = (QLineEdit*)ui->tableInfo->cellWidget(1 , 2);
    listValue << editColor->text();  //车辆颜色

    QComboBox *comBox = (QComboBox *)ui->tableInfo->cellWidget(2, 2);   //车辆类型
    listValue << comBox->currentText();

    comBox = (QComboBox *)ui->tableInfo->cellWidget(3, 2);  //燃油类型
    listValue << comBox->currentText();

    QSpinBox *spBox = (QSpinBox *)ui->tableInfo->cellWidget(4, 2);  //乘客数量
    listValue << spBox->text();

    spBox = (QSpinBox *)ui->tableInfo->cellWidget(5, 2);  //翻转角阈值
    listValue << spBox->text();

    comBox = (QComboBox *)ui->tableInfo->cellWidget(6, 2);  //3D是否初始化
    listValue << comBox->currentText();

    comBox = (QComboBox *)ui->tableInfo->cellWidget(7, 2);  //是否禁用六轴传感器
    listValue << comBox->currentText();

    title = listItem.join(",");
    content = listValue.join(",");
}

void MainWindow::on_btn_audio_result_clicked()
{
    QString path = ui->ledit_save_path->text();
    if(path.isEmpty())
    {
        QMessageBox::information(this, GET_LAN(0x0000005E),
                                 GET_LAN(0x00000062), QMessageBox::Ok);
        return;
    }
    else
    {
        path += tr("/TESTDATA/TEST RESULT");
    }

    dirExist(path);

    QEventLoop loop;
    QTimer::singleShot(500,&loop,SLOT(quit()));
    loop.exec();

    mutexSave.lock();

    //创建csv文件
    QString currentDate = QDateTime::currentDateTime().toString("yyyyMMdd");
    QString csvName = path + "/" +currentDate + ".csv";

    bool isFileExist = false;
    QFileInfo fileInfo(csvName);
    if(fileInfo.isFile())
    {
        isFileExist = true;
    }

    QString strTitle;
    QString strContent;
    makeSaveTest(strTitle, strContent);

    strTitle += QString(",%1").arg(GET_LAN(0x00000084));

    if (0x00 == callResult)
    {
        strContent += QString(",%1").arg("OK");
    }
    else if(0x01 == callResult)
    {
        strContent += QString(",%1").arg("NG");
    }
    else
    {
        strContent += QString(",");
    }
    
    //写入文件
    QFile file(csvName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        QMessageBox::information(this , GET_LAN(0x0000004A) ,
            QString(GET_LAN(0x00000087)).arg(csvName) , QMessageBox::Yes);
        mutexSave.unlock();
        return ;
    }
    QTextStream stream(&file);
    if(isFileExist)
    {
        stream << strContent << "\r\n";
    }
    else
    {
        stream << strTitle << "\r\n";
        stream << strContent << "\r\n";
    }
    file.flush();
    file.close();

    qWarning()<< GET_LAN(0x0000006A)  + ":" + csvName;

    mutexSave.unlock();

    emit signalResultSaveFinish();
}

void MainWindow::on_btn_command_load_clicked()
{
    QSettings settings("HPM10.ini", QSettings::IniFormat, this);
    //保存配置
    QString strPath = settings.value("path/command", ".").toString();

    QString s = QFileDialog::getOpenFileName(this,GET_LAN(0x0000006B),
                                             strPath,GET_LAN(0x0000006C));
    if(!s.isEmpty())
    {
        ui->ledit_save_path->setText(s);
        QSettings settings("HPM10.ini", QSettings::IniFormat, this);
        //保存配置
        settings.beginGroup("path");
        settings.setValue("command", s);
        settings.endGroup();
        initCommand(s);
    }
}

void MainWindow::on_btn_command_save_clicked()
{
    QSettings settings("HPM10.ini", QSettings::IniFormat, this);
    //保存配置
    QString strPath = settings.value("path/command", ".").toString();
    QString s = QFileDialog::getSaveFileName(this,GET_LAN(0x0000007C),strPath,
                                             GET_LAN(0x0000007D));
    if(!s.isEmpty())
    {
        QSettings settings("HPM10.ini", QSettings::IniFormat, this);
        //保存配置
        settings.beginGroup("path");
        settings.setValue("command", s);
        settings.endGroup();

        QFile file(s);
        if (!file.open(QFile::WriteOnly | QFile::Text))
        {
            qDebug() << "Open File Error.";
        }

        QDomDocument doc;
        doc.appendChild(doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\""));
        QDomElement project = doc.createElement(tr("project"));
        project.setAttribute(QString("name"),QString("HPM10"));
        project.setAttribute(QString("version"),QString("1.0"));

        int rowCnt = ui->tableCommand->rowCount();
        int colCnt = ui->tableCommand->columnCount();

        for(int t = 0; t < rowCnt; t++)
        {
            QStringList item;
            for(int i = 1; i < colCnt - 1; i++)
            {
                QLineEdit *edit = (QLineEdit *)ui->tableCommand->cellWidget(t, i);
                item.append(edit->text());
            }
            if(item.size() > 1)
            {
                QDomElement command = doc.createElement(tr("command"));
                command.setAttribute(QString("delay"),item[1]);
                command.setAttribute(QString("value"),item[0]);
                project.appendChild(command);
            }
        }
        doc.appendChild(project);
        QTextStream out(&file);
        doc.save(out, 4);
        file.close();
    }
}

void MainWindow::on_btn_command_start_clicked()
{
    if(timerVersion)
    {
        timerVersion->stop();
    }

    listCommand.clear();
    int rowCnt = ui->tableCommand->rowCount();
    int colCnt = ui->tableCommand->columnCount();

    for(int t = 0; t < rowCnt; t++)
    {
        QCheckBox *checkBox = (QCheckBox *)ui->tableCommand->cellWidget(t, 0);
        if(Qt::Checked == checkBox->checkState())
        {
            QStringList item;
            for(int i = 1; i < colCnt - 1; i++)
            {
                QLineEdit *edit = (QLineEdit *)ui->tableCommand->cellWidget(t, i);
                item.append(edit->text());
            }

            auto value = std::make_tuple(item[0], (quint32)item[1].toInt());
            listCommand.push_back(value);
        }
    }

    if(listCommand.isEmpty())
    {
        qWarning() << GET_LAN(0x00000075);
        return;
    }
    else
    {
        auto header = (CheckBoxHeaderView *)ui->tableCommand->horizontalHeader();
        emit header->setEnabledStatus(false);
        isExcuteCommand = true;
        enableCommandButtons(false);
        emit signalCommand();
    }
}

void MainWindow::on_btn_command_end_clicked()
{
    isExcuteCommand = false;
    ui->btn_command_clear->setEnabled(true);
    ui->btn_command_start->setEnabled(true);
    ui->btn_command_end->setEnabled(false);
    ui->btn_command_load->setEnabled(true);
    ui->btn_command_save->setEnabled(true);
    enableTabWidget(0xFFFF);
    enableHeadView(true);

    for(int t = 0; t < ui->tableCommand->rowCount(); t++)
    {
        QCheckBox *checkBox = (QCheckBox *)ui->tableCommand->cellWidget(t, 0);
        checkBox->setEnabled(true);
    }

    if(timerVersion)
    {
        timerVersion->start();
    }

    enableCommandButtons(true);
}

void MainWindow::on_btn_command_clear_clicked()
{
    int rowCnt = ui->tableCommand->rowCount();
    int colCnt = ui->tableCommand->columnCount();
    for(int t = 0; t < rowCnt; t++)
    {
        QCheckBox *checkBox = (QCheckBox *)ui->tableCommand->cellWidget(t, 0);
        checkBox->setChecked(false);

        for(int i = 1; i < colCnt - 1; i++)
        {
            QLineEdit *edit = (QLineEdit *)ui->tableCommand->cellWidget(t, i);
            edit->clear();
        }
    }
}

void MainWindow::on_cbx_language_currentIndexChanged(int index)
{
    if(isReboot)
    {
        int retBox = QMessageBox::information(this, GET_LAN(0x0000005E),
             GET_LAN(0x00000073), QMessageBox::Yes | QMessageBox::No);
        if (retBox == QMessageBox::Yes)
        {
            QSettings settings("HPM10.ini", QSettings::IniFormat, this);
            //保存配置
            settings.beginGroup("language");
            settings.setValue("index", index);
            settings.endGroup();
            emit onReboot();
        }
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    QSettings settings("HPM10.ini", QSettings::IniFormat, this);
    //保存配置
    settings.beginGroup("tab");
    settings.setValue("value", index);
    settings.endGroup();
}

void MainWindow::on_btn_call_start_clicked()
{
    QByteArray callItem(0x01 , 1);
    bool ret = hpm10->calltest(callItem);
    if (!ret)
    {
        qWarning() << "Call Test Failed";
    }
}

void MainWindow::on_btn_call_end_clicked()
{
    emit onCallFinish();
}
