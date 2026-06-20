#ifndef DBUSOBJECT_H
#define DBUSOBJECT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusMessage>

class DBusObject : public QObject {
    Q_OBJECT
public:
    explicit DBusObject(const QString& service, const QString& path,
               const QString& interface, bool systemBus = false,
               QObject* parent = nullptr);

    template<typename T>
    T callMethod(const QString& method);
    template<typename T>
    T property(const char* prop);
    void connectSignal(const QString& signalName, QObject* receiver, const char* slot);
    QStringList propertyNames();

private:
    QDBusInterface* m_interface = nullptr;
};
// TEMPLATED FUNCTIONS IMPLEMENTATION

template<typename T>
T DBusObject::callMethod(const QString& method) {
    QDBusReply<T> reply = m_interface->call(method);
    if (reply.isValid()) {
        return reply.value();
    }

    qWarning() << m_interface->service() << "method call failed:" << method;
    return T();
}

template<typename T>
T DBusObject::property(const char* prop) {
    QVariant reply = m_interface->property(prop);
    if (reply.isValid()) {
        return reply.value<T>();
    }

    qWarning() << m_interface->service() << "property call failed:" << prop;
    return T();
}
#endif // DBUSOBJECT_H
