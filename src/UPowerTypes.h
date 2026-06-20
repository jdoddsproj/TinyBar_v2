#ifndef UPOWERTYPES_H
#define UPOWERTYPES_H

const QString UPOWER_INTERFACE = "org.freedesktop.UPower";
const QString UPOWER_PATH = "/org/freedesktop/UPower";
const QString UPOWER_DEVICES = UPOWER_INTERFACE + ".Device";
const QString UPOWER_PROPERTIES = UPOWER_INTERFACE + ".Properties";
const QString DBUS_PROPERTIES = "org.freedesktop.DBus.Properties";

namespace UPowerTypes {
Q_NAMESPACE

enum Device {
    UnknownDevice,
    LinePower,
    Battery,
    UPS,
    Monitor
};
Q_ENUM_NS(Device)

enum State {
    UnknownState,
    Charging,
    Discharging, // When battery is NOT plugged in
    Empty,
    FullyCharged, // When fully charged and battery is plugged in
    PendingCharge,
    PendingDischarge
};

Q_ENUM_NS(State)
}
Q_DECLARE_METATYPE(UPowerTypes::Device)
Q_DECLARE_METATYPE(UPowerTypes::State)
#endif //UPOWERTYPES_H
