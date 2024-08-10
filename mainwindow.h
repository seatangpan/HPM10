#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "commhpm10.hpp"
#include <QMap>
#include <QDebug>
#include <QMutex>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    enum class cmdlist : quint16
    {
        CMD_REQ = 0x01,     //命令请求
        CMD_ANS,            //命令应答
    };

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void printBuildTime();

    void init();

    void enableUI(bool bTrue);

    void enableCommandButtons(bool bTrue);

    void enableHeadView(bool bTrue);

    void enableTabWidget(const quint16 &index);

    void initVersion();

    void updateVersion();

    void initInfo();

    void updateInfo();

    void initTest();

    void updateTest();

    void initCommand(const QString &filename);

    void appendMsgout(QString msg);

    void appendMsgcmd(QString msg, quint16 msgtype = 1);

    void enableDebug(bool bTrue);

    void resetTestUi();

private slots:

    void onSerialChanged();

    void onReboot();

    void onMsgRead(QByteArray buffer);

    void onConnectResp(quint8 result);

    void onDisConnectResp(quint8 result);

    void onGetVersionResp(quint8 result, QByteArray buffer);

    void onGetInfoResp(quint8 result, QByteArray buffer);

    void onSetInfoResp(quint8 result, QByteArray buffer);

    void onSelftestResp(quint8 result, QByteArray buffer);

    void onAudioResp(quint8 result, QByteArray buffer);

    void onCommandResp(quint16 result, QByteArray buffer);

    void onUpdateResp(quint16 result, QByteArray buffer);

    void onCallTest(quint8 result , QByteArray buffer);

    void onSerialDisconnectedResp();

    void onAudioStatus(quint8 status);

    void onAudioFinish();

    void onCallFinish();

    void onResultSaveFinish();

    void onCommand();

    void onSendCommand();

    void onUpdateSend();

    void onSelfTest();

private slots:

    void on_btn_comm_open_clicked();

    void on_btn_open_update_file_clicked();

    void on_btn_update_clicked();

    void on_btn_info_read_clicked();

    void on_btn_info_write_clicked();

    void on_btn_log_clear_clicked();

    void on_btn_log_save_clicked();

    void on_btn_test_start_clicked();

    void on_btn_test_end_clicked();

    void on_btn_audio_start_clicked();

    void on_btn_audio_end_clicked();

    void on_btn_save_path_select_clicked();

    void on_btn_test_suspend_clicked();

    void on_btn_save_test_result_clicked();

    void on_btn_info_save_clicked();

    void on_btn_audio_result_clicked();

    void on_btn_command_load_clicked();

    void on_btn_command_start_clicked();

    void on_btn_command_end_clicked();

    void on_btn_command_clear_clicked();

    void on_cbx_language_currentIndexChanged(int index);

    void on_btn_command_save_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_btn_call_start_clicked();

    void on_btn_call_end_clicked();

signals:

    void signalOutputLog(QString);

    void signalOutputCmd(QString);

    void signalClearLog();

    void signalAudioStatus(quint8 status);

    void signalAudioFinish();

    void signalCallFinish();

    void signalCommand();

    void sigalUpdateDataSend();

    void signalResultSaveFinish();

    void signalLostCommunication();

private:

    bool dirExist(const QString &path);

    void makeSaveInfo(QString &title,QString &content);

    void makeSaveAutoTest(QString &title,QString &content);

    void makeSaveTest(QString &title,QString &content);

public:

    static int EXIT_CODE_REBOOT;

    void signalReboot();

    bool isReboot{false};

private:
    Ui::MainWindow *ui;

    Commhpm10 *hpm10;

    bool isConnected{false};

    bool isAutotestStart{false};

    bool isDebugTrace{false};

    bool isUpdateTestUi{false};

    bool isExcuteCommand{false};

    QMutex mutexCommand;

    QMutex mutexSave;

    QMutex mutexUpdate;

    QMutex mutexOpen;

    QTimer *timerConnect{nullptr};

    QTimer *timerAutoTest{nullptr};

    QTimer *timerAudioStatus{nullptr};

    QTimer *timerVersion{nullptr};

    QList<std::tuple<QString,quint32>> listCommand;

    QList<QByteArray> listUpdateFrame;

    std::atomic<quint32> updateFrameCount{ 0 };
    std::atomic<quint32>  versionFrameCount{ 0 };

    QMap<quint8,QString> mapVersion;
    QMap<quint8,QString> mapInfo;
    QMap<quint8,QList<quint16>> mapSelftest;
    QList<QByteArray> listSelfTestFrame;

    quint32 autoInterval {5000};
    quint32 audioInterval {1000};
    quint32 versionInterval {5000};

    QByteArray testResult;
    quint8 audioResult { 0xFF };
    quint8 callResult { 0xFF };

    QString savePath;
    quint32 selectedTabIndex{0x00};
};

#endif // MAINWINDOW_H
