#include "DBusObject.h"
#include <QMetaObject>
#include <QMetaProperty>

DBusObject::DBusObject(const QString& service, const QString& path, const QString& interface, bool systemBus, QObject* parent)
    : QObject(parent)
{
    m_interface = new QDBusInterface(
        service,
        path,
        interface,
        systemBus ? QDBusConnection::systemBus() : QDBusConnection::sessionBus(),
        parent
    );

    if (!m_interface->isValid()) {
        qWarning() << "Failed to connect to D-Bus interface:"
                   << service << path << interface << systemBus;
    }
}

void DBusObject::connectSignal(const QString& signalName, QObject* receiver, const char* slot) {
    bool ok = QDBusConnection::systemBus().connect(
        m_interface->service(),
        m_interface->path(),
        "org.freedesktop.DBus.Properties",
        signalName,
        receiver,
        slot
    );

    if (!ok) {
        qWarning() << "DBusObject: Failed to connect signal to slot";
    }
}


QStringList DBusObject::propertyNames() {
    if (!m_interface->isValid()) {
        return QStringList();
    }

    const QMetaObject* mo = m_interface->metaObject();

    QStringList list;
    for (int i = mo->propertyOffset(); i < mo->propertyCount(); i++) {
        QMetaProperty prop = mo->property(i);
        list.append(prop.name());
    }

    return list;
}
