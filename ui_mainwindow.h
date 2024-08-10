/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <progressdisplaywidget.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout_10;
    QVBoxLayout *verticalLayout_2;
    QGridLayout *gridLayout_3;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *btn_save_path_select;
    QComboBox *cbx_language;
    QLabel *label;
    QSpacerItem *horizontalSpacer;
    QComboBox *cbx_comm_baudrate;
    QPushButton *btn_comm_open;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_4;
    QLineEdit *ledit_save_path;
    QLabel *label_2;
    QComboBox *cbx_comm_name;
    QSpacerItem *horizontalSpacer_10;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_7;
    QSpacerItem *horizontalSpacer_15;
    QTableWidget *tableVersion;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout_9;
    QGridLayout *gridLayout_8;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_3;
    QSpinBox *spinBox;
    QPushButton *btn_test_start;
    QPushButton *btn_test_suspend;
    QPushButton *btn_test_end;
    QPushButton *btn_save_test_result;
    QSpacerItem *horizontalSpacer_5;
    QTableWidget *tableTest;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btn_audio_start;
    QSpacerItem *horizontalSpacer_6;
    QPushButton *btn_audio_end;
    QSpacerItem *horizontalSpacer_7;
    ProgressDisplayWidget *widget;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *btn_call_start;
    QSpacerItem *horizontalSpacer_13;
    QPushButton *btn_call_end;
    QSpacerItem *horizontalSpacer_14;
    QPushButton *btn_audio_result;
    QWidget *tab_2;
    QGridLayout *gridLayout_6;
    QTableWidget *tableInfo;
    QHBoxLayout *horizontalLayout_11;
    QPushButton *btn_info_read;
    QPushButton *btn_info_write;
    QSpacerItem *horizontalSpacer_9;
    QPushButton *btn_info_save;
    QWidget *tab_3;
    QGridLayout *gridLayout_5;
    QHBoxLayout *horizontalLayout;
    QLabel *label_5;
    QSpinBox *spTime;
    QLabel *label_6;
    QSpinBox *spCount;
    QSpacerItem *horizontalSpacer_11;
    QPushButton *btn_command_start;
    QPushButton *btn_command_end;
    QPushButton *btn_command_load;
    QPushButton *btn_command_save;
    QPushButton *btn_command_clear;
    QSpacerItem *horizontalSpacer_12;
    QTableWidget *tableCommand;
    QWidget *tab_4;
    QGridLayout *gridLayout_7;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_9;
    QLineEdit *ledit_updatefile;
    QPushButton *btn_open_update_file;
    QHBoxLayout *horizontalLayout_10;
    QProgressBar *progressBar;
    QPushButton *btn_update;
    QSpacerItem *verticalSpacer;
    QGridLayout *gridLayout_2;
    QTextEdit *textEdit;
    QPushButton *btn_log_save;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *btn_log_clear;
    QHBoxLayout *horizontalLayout_4;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1127, 950);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        QFont font;
        font.setPointSize(9);
        centralWidget->setFont(font);
        gridLayout_10 = new QGridLayout(centralWidget);
        gridLayout_10->setSpacing(6);
        gridLayout_10->setContentsMargins(11, 11, 11, 11);
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setSpacing(6);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_3, 0, 5, 1, 1);

        btn_save_path_select = new QPushButton(centralWidget);
        btn_save_path_select->setObjectName(QString::fromUtf8("btn_save_path_select"));
        btn_save_path_select->setEnabled(true);

        gridLayout_3->addWidget(btn_save_path_select, 1, 5, 1, 1);

        cbx_language = new QComboBox(centralWidget);
        cbx_language->setObjectName(QString::fromUtf8("cbx_language"));
        cbx_language->setMinimumSize(QSize(80, 0));

        gridLayout_3->addWidget(cbx_language, 1, 9, 1, 1);

        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);

        gridLayout_3->addWidget(label, 0, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer, 0, 3, 1, 1);

        cbx_comm_baudrate = new QComboBox(centralWidget);
        cbx_comm_baudrate->setObjectName(QString::fromUtf8("cbx_comm_baudrate"));
        cbx_comm_baudrate->setMinimumSize(QSize(0, 0));

        gridLayout_3->addWidget(cbx_comm_baudrate, 0, 8, 1, 2);

        btn_comm_open = new QPushButton(centralWidget);
        btn_comm_open->setObjectName(QString::fromUtf8("btn_comm_open"));

        gridLayout_3->addWidget(btn_comm_open, 0, 10, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_2, 1, 10, 1, 1);

        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_3->addWidget(label_4, 1, 6, 1, 1);

        ledit_save_path = new QLineEdit(centralWidget);
        ledit_save_path->setObjectName(QString::fromUtf8("ledit_save_path"));
        ledit_save_path->setMinimumSize(QSize(0, 30));
        ledit_save_path->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        ledit_save_path->setDragEnabled(false);
        ledit_save_path->setClearButtonEnabled(true);

        gridLayout_3->addWidget(ledit_save_path, 1, 0, 1, 5);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_3->addWidget(label_2, 0, 6, 1, 1);

        cbx_comm_name = new QComboBox(centralWidget);
        cbx_comm_name->setObjectName(QString::fromUtf8("cbx_comm_name"));
        cbx_comm_name->setMinimumSize(QSize(100, 25));

        gridLayout_3->addWidget(cbx_comm_name, 0, 1, 1, 1);

        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_10, 0, 2, 1, 1);

        gridLayout_3->setColumnStretch(0, 4);

        verticalLayout_2->addLayout(gridLayout_3);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_8->addWidget(label_7);

        horizontalSpacer_15 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_15);


        verticalLayout_2->addLayout(horizontalLayout_8);

        tableVersion = new QTableWidget(centralWidget);
        tableVersion->setObjectName(QString::fromUtf8("tableVersion"));
        tableVersion->setEnabled(true);
        tableVersion->setAutoScrollMargin(0);
        tableVersion->horizontalHeader()->setDefaultSectionSize(25);
        tableVersion->verticalHeader()->setDefaultSectionSize(25);

        verticalLayout_2->addWidget(tableVersion);

        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        tab->setMinimumSize(QSize(0, 0));
        gridLayout_9 = new QGridLayout(tab);
        gridLayout_9->setSpacing(6);
        gridLayout_9->setContentsMargins(11, 11, 11, 11);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        gridLayout_8 = new QGridLayout();
        gridLayout_8->setSpacing(6);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_3 = new QLabel(tab);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_6->addWidget(label_3);

        spinBox = new QSpinBox(tab);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(spinBox->sizePolicy().hasHeightForWidth());
        spinBox->setSizePolicy(sizePolicy1);
        spinBox->setMinimumSize(QSize(60, 0));
        spinBox->setMaximum(1000000);
        spinBox->setSingleStep(100);
        spinBox->setValue(1);

        horizontalLayout_6->addWidget(spinBox);

        btn_test_start = new QPushButton(tab);
        btn_test_start->setObjectName(QString::fromUtf8("btn_test_start"));

        horizontalLayout_6->addWidget(btn_test_start);

        btn_test_suspend = new QPushButton(tab);
        btn_test_suspend->setObjectName(QString::fromUtf8("btn_test_suspend"));
        btn_test_suspend->setEnabled(false);

        horizontalLayout_6->addWidget(btn_test_suspend);

        btn_test_end = new QPushButton(tab);
        btn_test_end->setObjectName(QString::fromUtf8("btn_test_end"));
        btn_test_end->setEnabled(false);

        horizontalLayout_6->addWidget(btn_test_end);

        btn_save_test_result = new QPushButton(tab);
        btn_save_test_result->setObjectName(QString::fromUtf8("btn_save_test_result"));
        btn_save_test_result->setEnabled(false);

        horizontalLayout_6->addWidget(btn_save_test_result);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_5);


        gridLayout_8->addLayout(horizontalLayout_6, 0, 0, 1, 1);

        tableTest = new QTableWidget(tab);
        tableTest->setObjectName(QString::fromUtf8("tableTest"));
        tableTest->setMinimumSize(QSize(0, 0));
        tableTest->verticalHeader()->setMinimumSectionSize(20);
        tableTest->verticalHeader()->setDefaultSectionSize(25);

        gridLayout_8->addWidget(tableTest, 1, 0, 1, 1);

        groupBox = new QGroupBox(tab);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        btn_audio_start = new QPushButton(groupBox);
        btn_audio_start->setObjectName(QString::fromUtf8("btn_audio_start"));

        horizontalLayout_2->addWidget(btn_audio_start);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_6);

        btn_audio_end = new QPushButton(groupBox);
        btn_audio_end->setObjectName(QString::fromUtf8("btn_audio_end"));

        horizontalLayout_2->addWidget(btn_audio_end);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_7);


        gridLayout->addLayout(horizontalLayout_2, 0, 0, 1, 1);

        widget = new ProgressDisplayWidget(groupBox);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setEnabled(true);
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);
        widget->setMinimumSize(QSize(500, 60));
        widget->setStyleSheet(QString::fromUtf8(""));

        gridLayout->addWidget(widget, 1, 0, 1, 1);


        gridLayout_8->addWidget(groupBox, 2, 0, 1, 1);

        groupBox_2 = new QGroupBox(tab);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_4 = new QGridLayout(groupBox_2);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        btn_call_start = new QPushButton(groupBox_2);
        btn_call_start->setObjectName(QString::fromUtf8("btn_call_start"));

        horizontalLayout_3->addWidget(btn_call_start);

        horizontalSpacer_13 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_13);

        btn_call_end = new QPushButton(groupBox_2);
        btn_call_end->setObjectName(QString::fromUtf8("btn_call_end"));

        horizontalLayout_3->addWidget(btn_call_end);

        horizontalSpacer_14 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_14);

        btn_audio_result = new QPushButton(groupBox_2);
        btn_audio_result->setObjectName(QString::fromUtf8("btn_audio_result"));

        horizontalLayout_3->addWidget(btn_audio_result);


        gridLayout_4->addLayout(horizontalLayout_3, 0, 0, 1, 1);


        gridLayout_8->addWidget(groupBox_2, 3, 0, 1, 1);


        gridLayout_9->addLayout(gridLayout_8, 0, 0, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        gridLayout_6 = new QGridLayout(tab_2);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        tableInfo = new QTableWidget(tab_2);
        tableInfo->setObjectName(QString::fromUtf8("tableInfo"));
        tableInfo->setMinimumSize(QSize(0, 400));
        tableInfo->setMaximumSize(QSize(16777215, 16777215));

        gridLayout_6->addWidget(tableInfo, 1, 0, 1, 1);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        btn_info_read = new QPushButton(tab_2);
        btn_info_read->setObjectName(QString::fromUtf8("btn_info_read"));

        horizontalLayout_11->addWidget(btn_info_read);

        btn_info_write = new QPushButton(tab_2);
        btn_info_write->setObjectName(QString::fromUtf8("btn_info_write"));

        horizontalLayout_11->addWidget(btn_info_write);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer_9);

        btn_info_save = new QPushButton(tab_2);
        btn_info_save->setObjectName(QString::fromUtf8("btn_info_save"));

        horizontalLayout_11->addWidget(btn_info_save);


        gridLayout_6->addLayout(horizontalLayout_11, 0, 0, 1, 1);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        gridLayout_5 = new QGridLayout(tab_3);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_5 = new QLabel(tab_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout->addWidget(label_5);

        spTime = new QSpinBox(tab_3);
        spTime->setObjectName(QString::fromUtf8("spTime"));
        sizePolicy1.setHeightForWidth(spTime->sizePolicy().hasHeightForWidth());
        spTime->setSizePolicy(sizePolicy1);
        spTime->setMinimumSize(QSize(60, 0));
        spTime->setMaximum(1000000);
        spTime->setSingleStep(100);
        spTime->setValue(0);

        horizontalLayout->addWidget(spTime);

        label_6 = new QLabel(tab_3);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        horizontalLayout->addWidget(label_6);

        spCount = new QSpinBox(tab_3);
        spCount->setObjectName(QString::fromUtf8("spCount"));
        sizePolicy1.setHeightForWidth(spCount->sizePolicy().hasHeightForWidth());
        spCount->setSizePolicy(sizePolicy1);
        spCount->setMinimumSize(QSize(60, 0));
        spCount->setMaximum(10000);
        spCount->setSingleStep(100);
        spCount->setValue(1);

        horizontalLayout->addWidget(spCount);

        horizontalSpacer_11 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_11);

        btn_command_start = new QPushButton(tab_3);
        btn_command_start->setObjectName(QString::fromUtf8("btn_command_start"));

        horizontalLayout->addWidget(btn_command_start);

        btn_command_end = new QPushButton(tab_3);
        btn_command_end->setObjectName(QString::fromUtf8("btn_command_end"));

        horizontalLayout->addWidget(btn_command_end);

        btn_command_load = new QPushButton(tab_3);
        btn_command_load->setObjectName(QString::fromUtf8("btn_command_load"));

        horizontalLayout->addWidget(btn_command_load);

        btn_command_save = new QPushButton(tab_3);
        btn_command_save->setObjectName(QString::fromUtf8("btn_command_save"));

        horizontalLayout->addWidget(btn_command_save);

        btn_command_clear = new QPushButton(tab_3);
        btn_command_clear->setObjectName(QString::fromUtf8("btn_command_clear"));

        horizontalLayout->addWidget(btn_command_clear);

        horizontalSpacer_12 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_12);


        gridLayout_5->addLayout(horizontalLayout, 0, 0, 1, 1);

        tableCommand = new QTableWidget(tab_3);
        tableCommand->setObjectName(QString::fromUtf8("tableCommand"));
        tableCommand->setMinimumSize(QSize(0, 480));
        tableCommand->setMaximumSize(QSize(16777215, 16777215));

        gridLayout_5->addWidget(tableCommand, 1, 0, 1, 1);

        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        gridLayout_7 = new QGridLayout(tab_4);
        gridLayout_7->setSpacing(6);
        gridLayout_7->setContentsMargins(11, 11, 11, 11);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        ledit_updatefile = new QLineEdit(tab_4);
        ledit_updatefile->setObjectName(QString::fromUtf8("ledit_updatefile"));
        ledit_updatefile->setMinimumSize(QSize(0, 30));
        ledit_updatefile->setClearButtonEnabled(true);

        horizontalLayout_9->addWidget(ledit_updatefile);

        btn_open_update_file = new QPushButton(tab_4);
        btn_open_update_file->setObjectName(QString::fromUtf8("btn_open_update_file"));
        btn_open_update_file->setEnabled(false);

        horizontalLayout_9->addWidget(btn_open_update_file);


        verticalLayout->addLayout(horizontalLayout_9);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        progressBar = new QProgressBar(tab_4);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setMinimumSize(QSize(0, 40));
        progressBar->setValue(0);

        horizontalLayout_10->addWidget(progressBar);

        btn_update = new QPushButton(tab_4);
        btn_update->setObjectName(QString::fromUtf8("btn_update"));
        btn_update->setEnabled(false);
        btn_update->setMinimumSize(QSize(100, 40));

        horizontalLayout_10->addWidget(btn_update);


        verticalLayout->addLayout(horizontalLayout_10);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        gridLayout_7->addLayout(verticalLayout, 0, 0, 1, 1);

        tabWidget->addTab(tab_4, QString());

        verticalLayout_2->addWidget(tabWidget);

        verticalLayout_2->setStretch(3, 6);

        gridLayout_10->addLayout(verticalLayout_2, 0, 0, 1, 1);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(6);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        textEdit = new QTextEdit(centralWidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setMinimumSize(QSize(400, 0));
        textEdit->setFont(font);

        gridLayout_2->addWidget(textEdit, 0, 0, 1, 3);

        btn_log_save = new QPushButton(centralWidget);
        btn_log_save->setObjectName(QString::fromUtf8("btn_log_save"));

        gridLayout_2->addWidget(btn_log_save, 1, 0, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(37, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_4, 1, 1, 1, 1);

        btn_log_clear = new QPushButton(centralWidget);
        btn_log_clear->setObjectName(QString::fromUtf8("btn_log_clear"));

        gridLayout_2->addWidget(btn_log_clear, 1, 2, 1, 1);


        gridLayout_10->addLayout(gridLayout_2, 0, 1, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));

        gridLayout_10->addLayout(horizontalLayout_4, 1, 1, 1, 1);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "HPM10_TOOL_V1.0", nullptr));
        btn_save_path_select->setText(QCoreApplication::translate("MainWindow", "\351\200\211\346\213\251\344\277\235\345\255\230\347\273\223\346\236\234\350\267\257\345\276\204", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\344\270\262\345\217\243", nullptr));
        btn_comm_open->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\350\257\255\350\250\200", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\346\263\242\347\211\271\347\216\207", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "\347\273\210\347\253\257\345\217\202\346\225\260\350\257\273\345\217\226", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\350\207\252\345\212\250\346\265\213\350\257\225\346\254\241\346\225\260", nullptr));
        btn_test_start->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213", nullptr));
        btn_test_suspend->setText(QCoreApplication::translate("MainWindow", "\346\232\202\345\201\234", nullptr));
        btn_test_end->setText(QCoreApplication::translate("MainWindow", "\347\273\223\346\235\237", nullptr));
        btn_save_test_result->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\346\265\213\350\257\225\347\273\223\346\236\234", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\345\243\260\351\237\263\346\265\213\350\257\225", nullptr));
        btn_audio_start->setText(QCoreApplication::translate("MainWindow", "\345\220\257\345\212\250\346\265\213\350\257\225", nullptr));
        btn_audio_end->setText(QCoreApplication::translate("MainWindow", "\347\273\210\346\255\242\346\265\213\350\257\225", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "\351\200\232\350\257\235\346\265\213\350\257\225", nullptr));
        btn_call_start->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\346\265\213\350\257\225", nullptr));
        btn_call_end->setText(QCoreApplication::translate("MainWindow", "\347\273\223\346\235\237\346\265\213\350\257\225", nullptr));
        btn_audio_result->setText(QCoreApplication::translate("MainWindow", "\345\257\274\345\207\272\346\265\213\350\257\225\347\273\223\346\236\234", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("MainWindow", "\346\265\213\350\257\225\346\250\241\345\274\217", nullptr));
        btn_info_read->setText(QCoreApplication::translate("MainWindow", "\350\257\273\345\217\226", nullptr));
        btn_info_write->setText(QCoreApplication::translate("MainWindow", "\345\206\231\345\205\245", nullptr));
        btn_info_save->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230CSV\346\226\207\344\273\266", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("MainWindow", "\350\275\246\350\276\206\345\217\202\346\225\260\345\275\225\345\205\245", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\346\211\247\350\241\214\351\227\264\351\232\224", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "\346\211\247\350\241\214\346\254\241\346\225\260", nullptr));
        btn_command_start->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213", nullptr));
        btn_command_end->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242", nullptr));
        btn_command_load->setText(QCoreApplication::translate("MainWindow", "\345\212\240\350\275\275\346\265\213\350\257\225\351\241\271\350\204\232\346\234\254", nullptr));
        btn_command_save->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\346\265\213\350\257\225\350\204\232\346\234\254", nullptr));
        btn_command_clear->setText(QCoreApplication::translate("MainWindow", "\346\270\205\347\251\272", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QCoreApplication::translate("MainWindow", "\346\214\207\344\273\244\346\237\245\350\257\242", nullptr));
        btn_open_update_file->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200\345\215\207\347\272\247\346\226\207\344\273\266", nullptr));
        btn_update->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\345\215\207\347\272\247", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QCoreApplication::translate("MainWindow", "\347\273\210\347\253\257\345\215\207\347\272\247", nullptr));
        btn_log_save->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\346\227\245\345\277\227", nullptr));
        btn_log_clear->setText(QCoreApplication::translate("MainWindow", "\346\270\205\351\231\244\346\227\245\345\277\227", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
