#ifndef UPOWER_H
#define UPOWER_H

#include <QObject>
#include <QVariant>
#include <QVariantMap>
#include <QDBusInterface>
#include <QDBusObjectPath>
#include <QList>
#include <QString>
#include <QStringList>
#include "UPowerDevice.h"
#include "UPowerTypes.h"

class UPower : public QObject {
    Q_OBJECT
public:
    explicit UPower(QObject* parent = nullptr);

    QList<UPowerDevice*> enumerateDevices();
    UPowerDevice* getDevice(UPowerTypes::Device deviceType);
    void query(const QString& property);
    void queryAll();

signals:
    void chargeChanged(int charge); // Current battery charge from 0-100%
    void specialIncrement(int charge); // When battery charge is 10, 25, 50 or 75 %
    void stateChanged(int state); // Battery state (see State enum in UPowerType.h)
    void isPluggedIn(bool state);

private:
    QList<UPowerDevice*> m_deviceList;

    void onPropertiesChanged(UPowerDevice* device, const QVariantMap& changed);
};
#endif //UPOWER_H
