#include "UPowerDevice.h"
#include <QDBusConnection>
#include <QDBusReply>
#include <QVariant>
#include <QMetaObject>
#include <QMetaProperty>

UPowerDevice::UPowerDevice(const QString& service, const QString& path, const QString& interface, QObject* parent)
    : QObject(parent)
    , m_interface(new QDBusInterface(service, path, interface, QDBusConnection::systemBus(), parent))
{
    QDBusConnection::systemBus().connect(
        service,
        path,
        DBUS_PROPERTIES,
        "PropertiesChanged",
        this,
        SLOT(onPropertiesChanged(QString, QVariantMap, QStringList))
    );
}


UPowerTypes::Device UPowerDevice::type() {
    if (m_type != UPowerTypes::UnknownDevice) {
        return m_type;
    };

    QVariant reply = m_interface->property("Type");
    if (reply.isValid()) {
        UPowerTypes::Device type = static_cast<UPowerTypes::Device>(reply.toUInt());
        if (type != UPowerTypes::UnknownDevice) {
            m_type = type;
            return m_type;
        };
    };

    return UPowerTypes::UnknownDevice;
}


QStringList UPowerDevice::propertyNames() {
    const QMetaObject* mo = m_interface->metaObject();

    QStringList list;
    for (int i = mo->propertyOffset(); i < mo->propertyCount(); i++) {
        QMetaProperty prop = mo->property(i);
        list.append(prop.name());
        //qDebug() << prop.name();
    }
    return list;
}


void UPowerDevice::onPropertiesChanged(const QString& path, const QVariantMap& changed, const QStringList& sl) {
    Q_UNUSED(path);
    Q_UNUSED(sl);
    emit propertiesChanged(this, changed);
}


