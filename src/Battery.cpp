#include "Battery.h"
#include <QPainterPath>
#include <QVariantMap>

Battery::Battery(float w, float h, QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);

    setWidth(w);
    setHeight(h);
    setPadding(DEFAULT_PADDING);
    setRounding(DEFAULT_ROUNDING);
    setChargedSideColor(DEFAULT_COLOR);
    setEmptySideColor(DEFAULT_EMPTY_COLOR);
    setBatteryLowColor(DEFAULT_LOW_COLOR);
    setIsChargingColor(DEFAULT_CHARGING_COLOR);
    setTextColor(DEFAULT_TEXT_COLOR);
    setFont(DEFAULT_FONT);
    m_font.setStyleStrategy(QFont::PreferAntialias);

    transparentText(false);

    setFixedSize(m_width + m_padding * 2.0, m_height + m_padding * 2.0);

    DBusObject* displayDevice = new DBusObject(
        DEFAULT_U_POWER_IFACE,
        DISPLAY_DEVICE_DEVICE_PATH,
        DEFAULT_U_POWER_DEVICE,
        true,
        this
    );

    displayDevice->connectSignal(
        "PropertiesChanged",
        this,
        SLOT(handleProperties(QString, QVariantMap, QStringList))
    );

    updateBatteryLevel(displayDevice->property<float>("Percentage"));
    isCharging(displayDevice->property<BatteryState>("State"));
}

// PROTECTED
void Battery::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    QRectF rect(m_padding, m_padding, m_width, m_height);

    int midX = rect.x();
    int divideLine = midX + qRound(rect.width() * m_percent);

    // Left side of rect (% charged)
    const QColor lhsColor = getBatteryStateColor();
    painter.save();
    painter.setClipRect(QRect(rect.x(), rect.y(), (divideLine - rect.x()), rect.height()));
    painter.setBrush(lhsColor);
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
    painter.save();
    painter.setCompositionMode(m_textCompMode);
    painter.setFont(m_font);
    painter.setPen(m_textColor);
    painter.drawText(rect, Qt::AlignCenter, (QString::number(m_percent * 100.0)));
    painter.restore();
}

// PUBLIC
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

void Battery::setChargedSideColor(const QColor& color) {
    m_color = color;
    update();
}

void Battery::setBatteryLowColor(const QColor& color) {
    m_batteryLowColor = color;
    update();
}

void Battery::setEmptySideColor(const QColor& color) {
    m_emptyColor = color;
    update();
}

void Battery::setIsChargingColor(const QColor& color) {
    m_chargingColor = color;
    update();
}

void Battery::setTextColor(const QColor& color) {
    m_textColor = color;
    update();
}

void Battery::setFont(const QFont& font) {
    m_font = font;
    update();
}

void Battery::transparentText(bool transparent) {
    if (transparent) {
        m_textCompMode = QPainter::CompositionMode_DestinationOut;
    }
    else {
        m_textCompMode = QPainter::CompositionMode_SourceOver;
    }
}

// SLOTS
void Battery::handleProperties(const QString& path, const QVariantMap& changed, const QStringList& invalidated) {
    Q_UNUSED(path);
    Q_UNUSED(invalidated);

    if (changed.contains("Percentage")) {
        updateBatteryLevel(changed["Percentage"].toFloat());
    }
    else if (changed.contains("State")) {
        isCharging(changed["State"].toUInt());
    }
}

void Battery::updateBatteryLevel(float percent) {
    m_percent = percent / 100.0;
    update();
}

void Battery::isCharging(quint8 state) {
    if (state == BatteryState::Charging or state == BatteryState::PendingCharge or state == BatteryState::FullyCharged) {
        m_isCharging = true;
    }
    else {
        m_isCharging = false;
    }
    update();
}

// PRIVATE
const QColor& Battery::getBatteryStateColor() const {
    if (m_isCharging) {
        return m_chargingColor;
    }
    if (m_percent <= qBound(0.00f, LOW_THRESHOLD, 1.00f)) {
        return m_batteryLowColor;
    }
    return m_color;
}




