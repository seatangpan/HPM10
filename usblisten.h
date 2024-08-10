#ifndef USBLISTEN_H
#define USBLISTEN_H

#include <QWidget>
#include <windows.h>
#include <QAbstractNativeEventFilter>
#include <dbt.h>

class UsbListen : public QWidget, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit UsbListen(QWidget *parent = 0);

    void EmitMySignal();

signals:

    void DeviceChangeCbk();
    void DevicePlugIn();
    void DevicePlugOut();

protected:

    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);

public slots:

};

#endif // USBLISTEN_H
