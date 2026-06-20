#ifndef BATTERY_H
#define BATTERY_H

#include <QWidget>
#include <QString>
#include <QPainter>
#include <QPaintEvent>
#include <QColor>
#include <QtGlobal>
#include <QPropertyAnimation>
#include <QLinearGradient>
#include "DBusObject.h"

#define DEFAULT_PADDING 4.0
#define DEFAULT_ROUNDING 5.0

#define LOW_THRESHOLD 0.20 // 0.00-1.00
#define DEFAULT_COLOR Qt::white
#define DEFAULT_LOW_COLOR QColor(255, 42, 0) // orangish red
#define DEFAULT_CHARGING_COLOR Qt::green
#define DEFAULT_EMPTY_COLOR Qt::lightGray
#define DEFAULT_TEXT_COLOR QColor(102, 102, 89) // Dark gray

#define DEFAULT_FONT QFont("Inter", 11)

#define DEFAULT_U_POWER_IFACE "org.freedesktop.UPower"
#define DEFAULT_U_POWER_PATH "/org/freedesktop/UPower/devices"
#define DEFAULT_U_POWER_DEVICE "org.freedesktop.UPower.Device"
#define BATTERY_DEVICE_PATH "/org/freedesktop/UPower/devices/battery_BAT0"
#define CHARGER_DEVICE_PATH "/org/freedesktop/UPower/devices/line_power_AC"


class Battery : public QWidget {
    Q_OBJECT
public:
    friend class TestBattery;
    explicit Battery(float w, float h, QWidget* parent = nullptr);

    void setWidth(float w);
    void setHeight(float h);
    void setPadding(float p);
    void setRounding(float r);

    void setChargedSideColor(QColor color);
    void setEmptySideColor(QColor color);
    void setBatteryLowColor(QColor color);
    void setIsChargingColor(QColor color);
    void setTextColor(QColor color);

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void handleProperties(const QString& path, const QVariantMap& changed, const QStringList& invalidated);
    void updateBatteryLevel(float percent);
    void isCharging(bool charging);

private:
    float m_width = 0.0;
    float m_height = 0.0;
    float m_padding = 0.0;
    float m_rounding = 0.0;

    float m_percent = 0.0;
    bool m_isCharging = false;

    QColor m_color;
    QColor m_emptyColor;
    QColor m_batteryLowColor;
    QColor m_chargingColor;
    QColor m_textColor;
};

#endif // BATTERY_H


// NOTES
