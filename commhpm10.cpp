#include "commhpm10.hpp"

Commhpm10::Commhpm10()
{
}

Commhpm10::~Commhpm10()
{

}

QList<QSerialPortInfo> Commhpm10::getCommNames()
{
    QList<QSerialPortInfo> list_avail_sp;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);

        //if (serial.open(QIODevice::ReadWrite))
        {
            list_avail_sp.append(info);
            serial.close();
        }
    }
    return list_avail_sp;
}

QStringList Commhpm10::getCommBps()
{
    return listBuadrate;
}

qint32 Commhpm10::getCommBps(const QString &sValue)
{
    foreach (const qint32 &var, listBuadrateValue)
    {
        if(var == sValue.toInt())
        {
            return var;
        }
    }
    return QSerialPort::UnknownBaud;
}

bool Commhpm10::open(const QString &sPort, const qint32 &nBaudrate)
{
    if(bOpen)
    {
        close();
    }
    serialPort.setPortName(sPort);
    serialPort.setBaudRate(nBaudrate);
    bOpen = serialPort.open(QIODevice::ReadWrite);
    serialPort.setDataTerminalReady(true);
    QObject::connect(&serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
    QObject::connect(&serialPort, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),this, &Commhpm10::handleSerialError);
    qDebug() << QString("serial port:%1,Buadrate:%2, result:%3").arg(sPort).arg(nBaudrate).arg(bOpen?"OK":"NG");
    return bOpen;
}

void Commhpm10::close()
{
    QObject::disconnect(&serialPort, 0, this, 0);//解除信号槽
    if(bOpen)
    {
        serialPort.clear();
        serialPort.close();
    }
    bOpen = false;
}

bool Commhpm10::connectReq()
{
    QByteArray empty;
    return writeData(paramlist::HP10_CONNECT, empty);
}

bool Commhpm10::disconnectReq()
{
    QByteArray empty;
    return writeData(paramlist::HP10_DISCONNECT, empty);
}

bool Commhpm10::getVersion(const QByteArray &buffer)
{
    return writeData(paramlist::HP10_READ_VERSION, buffer);
}

bool Commhpm10::getInfo(const QByteArray &buffer)
{
    return writeData(paramlist::HP10_READ_INFO, buffer);
}

bool Commhpm10::setInfo(const QByteArray &buffer)
{
    return writeData(paramlist::HP10_WRIET_INFO, buffer);
}

bool Commhpm10::selfTest(const QByteArray &buffer)
{
    return writeData(paramlist::HP10_SELFCHECK, buffer);
}

bool Commhpm10::cmdTest(const QByteArray &buffer)
{
    return writeData(paramlist::HP10_TESTMODE, buffer);
}

bool Commhpm10::audioTest(const QByteArray &buffer)
{
    return writeData(paramlist::HP10_VOICE_TEST, buffer);
}

bool Commhpm10::update(const QByteArray &buffer)
{
    return writeData(paramlist::HP10_UPDATE, buffer);
}

bool Commhpm10::calltest(const QByteArray& buffer)
{
    return writeData(paramlist::HP10_CALLTEST , buffer);
}

QByteArray Commhpm10::sendRecv(Commhpm10::paramlist para, const QByteArray &buffer)
{
    const quint8 protocolVersion {0x14};
    const quint8 head[2] = { 0x23, 0x23};
    quint16 len = buffer.size();
    quint16 sendSize = len + 10;

    //23 23 00 0A 14 00 01 00 01 1E
    quint8 *transData = new quint8[sendSize];
    memcpy(transData, head, sizeof(head)); 
    transData[2] = (sendSize >> 8) & 0xFF;
    transData[3] = sendSize & 0xFF;
    transData[4] = protocolVersion;
    transData[5] = (seq >> 8) & 0xFF;
    transData[6] = seq & 0xFF;
    transData[7] = (quint16(para) >> 8) & 0xFF;
    transData[8] = quint16(para) & 0xFF;
    if(len > 0)
    {
        memcpy(transData + 9, buffer.data(), len );
    }

    transData[sendSize - 1] = xorChecksum( transData + 2, sendSize - 3);
    seq++;

    QByteArray sendData((char *)transData, sendSize);
    qDebug() << "Send Data:" << binary2String(sendData);
    serialPort.write(sendData);

    QByteArray result;
    serialPort.waitForBytesWritten(5000);
    while(serialPort.waitForReadyRead(3000))
    {
        QByteArray recvdata = serialPort.readAll(); //读取串口数据
        quint8 *buffer = (quint8 *)recvdata.data();
        if(!recvdata.isEmpty())
        {
            quint16 i = 0;
            while(i < recvdata.size() - 4)
            {
                if(0 == memcmp(head, buffer + i,sizeof(head)))
                {
                    quint16 len = buffer[i + 2] * 0x100 + buffer[i + 3];
                    if(len <= recvdata.size())
                    {
                        if(protocolVersion == buffer[i + 4])
                        {
                            quint8 checksum = xorChecksum(buffer + i + 2, len - 3);
                            if(checksum == buffer[i + len - 1])
                            {
                                result = QByteArray((char *)(buffer + i), len);
                                break;
                            }
                        }
                    }
                }
                i++;
            }
        }

        if(!result.isEmpty())
        {
            qDebug() << "Recv Data:" << binary2String(result);
            break;
        }
    }
    delete transData;
    return result;
}

void Commhpm10::handleSerialError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError || error == QSerialPort::PermissionError)
    {
        qDebug()<<"comm disconnected...";
        emit signalSerialDisconnected();
    }
}

bool Commhpm10::writeData(Commhpm10::paramlist para, const QByteArray &buffer)
{
    const quint8 protocolVersion {0x14};
    const quint8 head[2] = { 0x23, 0x23};
    quint16 len = buffer.size();
    quint16 sendSize = len + 10;

    //23 23 00 0A 14 00 01 00 01 1E
    quint8 *transData = new quint8[sendSize];
    memcpy(transData, head, sizeof(head));
    transData[2] = (sendSize >> 8) & 0xFF;
    transData[3] = sendSize & 0xFF;
    transData[4] = protocolVersion;
    transData[5] = (seq >> 8) & 0xFF;
    transData[6] = seq & 0xFF;
    transData[7] = (quint16(para) >> 8) & 0xFF;
    transData[8] = quint16(para) & 0xFF;
    if(len > 0)
    {
        memcpy(transData + 9, buffer.data(), len );
    }

    transData[sendSize - 1] = xorChecksum( transData + 2, sendSize - 3);
    seq++;

    QByteArray sendData((char *)transData, sendSize);
    qDebug() << "SendData:" << binary2String(sendData);

    mutex.lock();
    qint16 ret = 0;
    if(bOpen)
    {
        ret = serialPort.write(sendData);
    }
    mutex.unlock();

    delete[] transData;
    return (ret > 0);
}

void Commhpm10::readData()
{
    const quint8 protocolVersion {0x14};
    const quint8 head[2] = { 0x23, 0x23};
    QByteArray frameData = serialPort.readAll();

    quint8 *buffer = (quint8 *)frameData.data();
    QList<QByteArray> listFrames;
    qint16 i = 0;

    //如果快速发送，则需要分包，当前未发现需要在多帧数据取值的问题
    while(i < frameData.size() - 4)
    {
        if(0 == memcmp(head, buffer + i,sizeof(head)))
        {
            quint16 len = buffer[i + 2] * 0x100 + buffer[i + 3];
            if(len <= frameData.size())
            {
                if(protocolVersion == buffer[i + 4])
                {
                    quint8 checksum = xorChecksum(buffer + i + 2, len - 3);
                    if(checksum == buffer[i + len - 1])
                    {
                        listFrames.push_back(QByteArray((char *)(buffer + i), len));
                        i += len;
                        continue;
                    }
                }
            }
        }
        i++;
    }

    while (listFrames.size())
    {
        QByteArray oneItem = listFrames.front();
        qDebug() << "recvData:" << binary2String(oneItem);
        signalMsgRead(oneItem);
        listFrames.pop_front();
    }
}

quint8 Commhpm10::xorChecksum(quint8 *data, quint16 len)
{
    quint8 check_sum = 0;
    while (len--)
    {
        check_sum ^= *data++;
    }
    return check_sum;
}

QString Commhpm10::binary2String(const QByteArray &buffer)
{
    QString sBuffer;
    for (int i = 0; i < buffer.size(); i++)
    {
        QString s;
        sBuffer += s.sprintf("%02X ", buffer.at(i) & 0xFF);
    }
    return sBuffer;
}
