#ifndef UPOWERDEVICE_H
#define UPOWERDEVICE_H

#include <QDBusInterface>
#include <QVariantMap>
#include <QStringList>
#include <QString>
#include <QStringList>
#include "UPowerTypes.h"

class UPowerDevice : public QObject {
    Q_OBJECT
public:
    explicit UPowerDevice(const QString& service, const QString& path, const QString& interface, QObject* parent = nullptr);
    UPowerTypes::Device type();
    template<typename T>
    T queryProperty(const char* prop);
    QStringList propertyNames();

signals:
    void propertiesChanged(UPowerDevice* device, const QVariantMap& changed);

private slots:
    void onPropertiesChanged(const QString& path, const QVariantMap& changed, const QStringList& invalidated);

private:
    QDBusInterface* m_interface = nullptr;
    UPowerTypes::Device m_type = UPowerTypes::UnknownDevice;
};

// TEMPLATE IMPLEMENTATIONS
template<typename T>
T UPowerDevice::queryProperty(const char* prop) {
    QVariant reply = m_interface->property(prop);
    if (!reply.isValid()) {
        qWarning() << "Invalid property query:" << prop;
        return T();
    };
    return reply.value<T>();
}
#endif //UPOWERDEVICE_H
