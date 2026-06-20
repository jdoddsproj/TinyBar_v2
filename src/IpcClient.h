#pragma once

#include <QObject>
#include <QString>

class IpcClient : public QObject {
    Q_OBJECT
public:
    using QObject::QObject;

    virtual void connectToLocalSocket(const QString& path) = 0;
    virtual void sendCommand(const QString& cmd) = 0;

signals:
    void eventReceived(const QString& event);
    void responseReceived(const QString& response);
};
