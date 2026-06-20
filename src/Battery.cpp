#include "Battery.h"
#include <QPainterPath>
#include <QVariantMap>

Battery::Battery(float w, float h, QWidget* parent)
    : QWidget(parent)
{
    setWidth(w);
    setHeight(h);
    setPadding(DEFAULT_PADDING);
    setRounding(DEFAULT_ROUNDING);
    setChargedSideColor(DEFAULT_COLOR);
    setEmptySideColor(DEFAULT_EMPTY_COLOR);
    setBatteryLowColor(DEFAULT_LOW_COLOR);
    setIsChargingColor(DEFAULT_CHARGING_COLOR);
    setTextColor(DEFAULT_TEXT_COLOR);

    setFixedSize(m_width + m_padding * 2.0, m_height + m_padding * 2.0);


    DBusObject* battery = new DBusObject(
        DEFAULT_U_POWER_IFACE,
        BATTERY_DEVICE_PATH,
        DEFAULT_U_POWER_DEVICE,
        true,
        this
    );
    DBusObject* charger = new DBusObject(
        DEFAULT_U_POWER_IFACE,
        CHARGER_DEVICE_PATH,
        DEFAULT_U_POWER_DEVICE,
        true,
        this
        );

    battery->connectSignal(
        "PropertiesChanged",
        this,
        SLOT(handleProperties(QString, QVariantMap, QStringList))
    );
    charger->connectSignal(
        "PropertiesChanged",
        this,
        SLOT(handleProperties(QString, QVariantMap, QStringList))
        );

    updateBatteryLevel(battery->property<float>("Percentage"));
    isCharging(charger->property<bool>("Online"));
}

void Battery::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    QPainterPath path;
    QRectF rect(m_padding, m_padding, m_width, m_height);

    int midX = rect.x();
    int divideLine = midX + qRound(rect.width() * m_percent);

    // Left side of rect (% charged)
    QColor color = m_isCharging ? m_chargingColor : (m_percent <= qBound(0.00f, LOW_THRESHOLD, 1.00f)) ? m_batteryLowColor : m_color;
    painter.save();
    painter.setClipRect(QRect(rect.x(), rect.y(), (divideLine - rect.x()), rect.height()));
    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, m_rounding, m_rounding);
    painter.restore();

    // Right side of rect (% empty)
    painter.save();
    painter.setClipRect(QRect(divideLine, rect.y(), rect.right() - midX + 1, rect.height()));
    painter.setBrush(m_emptyColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, m_rounding, m_rounding);
    painter.restore();

    // Draw battery percent text
    QFont font = DEFAULT_FONT;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setBold(true);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    painter.setFont(font);
    painter.setPen(m_textColor);
    painter.drawText(rect, Qt::AlignCenter, (QString::number(m_percent * 100.0)));

    // Draw sheen (WIP)
    /*
    path.addRoundedRect(rect, m_rounding, m_rounding);
    painter.setClipPath(path);

    QLinearGradient sheenGrad(midX - rect.width(), rect.top(), midX + rect.width() * 0.3, rect.bottom());
    sheenGrad.setColorAt(0.0, QColor(255, 255, 255, 0));
    sheenGrad.setColorAt(0.5, QColor(255, 255, 255, 70));
    sheenGrad.setColorAt(1.0, QColor(255, 255, 255, 0));

    painter.fillRect(rect, sheenGrad);
    */
}

void Battery::setWidth(float w) {
    if (w < 1.0) {
        w = 1.0;
    }
    m_width = w;
    update();
}

void Battery::setHeight(float h) {
    if (h < 1.0) {
        h = 1.0;
    }
    m_height = h;
    update();
}

void Battery::setPadding(float p) {
    if (p < 0.0) {
        p = 0.0;
    }
    m_padding = p;
    update();
}

void Battery::setRounding(float r) {
    if (r < 0.0) {
        r = 0.0;
    }
    m_rounding = r;
    update();
}

void Battery::setChargedSideColor(QColor color) {
    m_color = color;
    update();
}

void Battery::setBatteryLowColor(QColor color) {
    m_batteryLowColor = color;
    update();
}

void Battery::setEmptySideColor(QColor color) {
    m_emptyColor = color;
    update();
}

void Battery::setIsChargingColor(QColor color) {
    m_chargingColor = color;
    update();
}

void Battery::setTextColor(QColor color) {
    m_textColor = color;
    update();
}


void Battery::handleProperties(const QString& path, const QVariantMap& changed, const QStringList& invalidated) {
    Q_UNUSED(path);
    Q_UNUSED(invalidated);

    if (changed.contains("Percentage")) {
        float percent = changed["Percentage"].toFloat();
        updateBatteryLevel(percent);
    }
    else if (changed.contains("Online")) {
        isCharging(changed["Online"].toBool());
    }
}

void Battery::updateBatteryLevel(float percent) {
    m_percent = percent / 100.0;
    update();
}

void Battery::isCharging(bool charging) {
    m_isCharging = charging;
    update();
}


