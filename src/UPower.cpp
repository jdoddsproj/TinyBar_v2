#include "UPower.h"
#include "DBusObject.h"
#include <QDBusConnection>
#include <QDBusReply>
#include <QMetaEnum>
#include <QList>

UPower::UPower(QObject* parent)
    : QObject(parent)
{
    m_deviceList = enumerateDevices();

    UPowerDevice* battery = getDevice(UPowerTypes::Battery);
    UPowerDevice* charger = getDevice(UPowerTypes::LinePower);

    for (const auto& prop : battery->propertyNames()) {
        qDebug() << prop;
    }

    connect(battery, &UPowerDevice::propertiesChanged, this, &UPower::onPropertiesChanged);
    connect(charger, &UPowerDevice::propertiesChanged, this, &UPower::onPropertiesChanged);

    DBusObject* obj = new DBusObject(
        UPOWER_INTERFACE,
        UPOWER_PATH,
        UPOWER_INTERFACE,
        true,
        this
    );

    auto response = obj->callMethod<QList<QDBusObjectPath>>("EnumerateDevices");
    for (const auto& r : response) {
        qDebug() << r.path();
    }

    DBusObject* obj2 = new DBusObject(
        UPOWER_INTERFACE,
        UPOWER_PATH + "/devices/battery_BAT0",
        UPOWER_DEVICES,
        true,
        this
    );

    auto resp = obj2->property<QString>("Type");
    qDebug() << resp;

    auto resp2 = obj2->property<QString>("Percentage");
    qDebug() << resp2;
}


QList<UPowerDevice*> UPower::enumerateDevices() {
    QDBusInterface upower(
        UPOWER_INTERFACE,
        UPOWER_PATH,
        UPOWER_INTERFACE,
        QDBusConnection::systemBus()
    );
    if (!upower.isValid()) {
        qWarning() << "UPower service is not available";
    };

    QDBusReply<QList<QDBusObjectPath>> reply = upower.call("EnumerateDevices");
    if (!reply.isValid()) {
        qWarning() << "EnumerateDevices failed:" << reply.error().message();
    };

    QList<QDBusObjectPath> devicePaths = reply.value();
    QList<UPowerDevice*> devices;
    for (const QDBusObjectPath& path : devicePaths) {
        UPowerDevice* device = new UPowerDevice(
            UPOWER_INTERFACE,
            path.path(),
            UPOWER_DEVICES,
            this
        );
        devices.append(device);
    };

    return devices;
}

UPowerDevice* UPower::getDevice(UPowerTypes::Device deviceType) {
    if (m_deviceList.isEmpty()) {
        qWarning() << "No devices found";
        return nullptr;
    };

    for (UPowerDevice* device : m_deviceList) {
        if (deviceType == device->type()) {
            return device;
        };
    };

    qWarning() << "UPowerDevice: Device type" << QString(QMetaEnum::fromType<UPowerTypes::Device>().valueToKey(deviceType)) << "not found.";
    return nullptr;
}


void UPower::query(const QString& property) {
    return;
}

void UPower::queryAll() {
    UPowerDevice* battery = getDevice(UPowerTypes::Battery);
    UPowerDevice* charger = getDevice(UPowerTypes::LinePower);

    emit chargeChanged(battery->queryProperty<int>("Percentage"));
    emit isPluggedIn(charger->queryProperty<bool>("Online"));
}


void UPower::onPropertiesChanged(UPowerDevice* device, const QVariantMap& changed) {
    if (!device) {
        return;
    };

    switch (device->type()) {
        case UPowerTypes::LinePower:
            if (changed.contains("Online")) {
                bool state = changed["Online"].toBool();
                emit isPluggedIn(state);
            };
            break;

        case UPowerTypes::Battery:
            if (changed.contains("Percentage")) {
                int charge = changed["Percentage"].toInt();
                emit chargeChanged(charge);
                if (!(charge % 25) or charge == 10) {
                    emit specialIncrement(charge);
                };
            };
            if (changed.contains("State")) {
                UPowerTypes::State state = static_cast<UPowerTypes::State>(changed["State"].toInt());
                emit stateChanged(state);
            };
            break;

        case UPowerTypes::UPS:
            break;

        case UPowerTypes::Monitor:
            break;
    };
}
