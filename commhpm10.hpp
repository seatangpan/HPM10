#ifndef COMMHP100_HPP
#define COMMHP100_HPP

#include <QtSerialPort/QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QStringList>
#include <QMutex>

class Commhpm10  : public QObject
{
    Q_OBJECT

public:

    enum class paramlist : quint16
    {
        HP10_CONNECT = 0x01,    //连接请求
        HP10_DISCONNECT,        //断开请求
        HP10_READ_VERSION,      //获取终端参数
        HP10_WRITE_VERSION,     //设置终端参数
        HP10_READ_INFO,         //获取车辆参数
        HP10_WRIET_INFO,        //设置车辆参数
        HP10_UPDATE,            //终端升级
        HP10_SELFCHECK,         //测试结果查询
        HP10_VOICE_TEST,        //声音测试
        HP10_REBOOT,            //终端重启
        HP10_TESTMODE,          //测试命令
        HP10_CALLTEST,          //通话命令
    };

    Commhpm10();
    ~Commhpm10();

    //获取串口名称
    QList<QSerialPortInfo> getCommNames();

    //获取串口波特率列表
    QStringList getCommBps();

    //获取波特率的值
    qint32 getCommBps(const QString &sValue);

    bool isOpen() const {return bOpen;}

    quint16 getSeqence(){return seq;}

    //打开串口
    bool open(const QString &sPort, const qint32 &nBaudrate);

    //关闭串口
    void close();

    //连接请求
    bool connectReq();

    //断开连接请求
    bool disconnectReq();

    //获取终端参数
    bool getVersion(const QByteArray &buffer);

    //设置终端参数
    bool setVersion(const QByteArray &buffer);

    //获取车辆参数
    bool getInfo(const QByteArray &buffer);

    //设置车辆参数
    bool setInfo(const QByteArray &buffer);

    //测试结果查询
    bool selfTest(const QByteArray &buffer);

    //测试命令
    bool cmdTest(const QByteArray &buffer);

    //声音测试
    bool audioTest(const QByteArray &buffer);

    //升级
    bool update(const QByteArray &buffer);

    //通话测试
    bool calltest(const QByteArray& buffer);

    QString binary2String(const QByteArray &buffer);

protected:

    QByteArray sendRecv(paramlist para, const QByteArray &buffer);

    quint8 xorChecksum(quint8 *data, quint16 len);

public slots:

    //串口断开
    void handleSerialError(QSerialPort::SerialPortError error);

    void readData();

    bool writeData(Commhpm10::paramlist para, const QByteArray &buffer);

signals:

    //串口断开
    void signalSerialDisconnected(void);

    //波特率错误
    void signalSerialCommError(void);

    //返回应答的报文
    void signalMsgRead(QByteArray buffer);

private:

    QMutex mutex;

    QStringList listBuadrate{"1200","2400","4800","9600","19200", "38400", "57600","115200"};

    QList<qint32> listBuadrateValue
    {
        QSerialPort::Baud1200, QSerialPort::Baud2400, QSerialPort::Baud4800,
        QSerialPort::Baud9600, QSerialPort::Baud115200, QSerialPort::UnknownBaud
    };

    QSerialPort serialPort;
    bool bOpen {false};

    quint16 seq { 0x0001 };
};

#endif // COMMHP100_HPP
