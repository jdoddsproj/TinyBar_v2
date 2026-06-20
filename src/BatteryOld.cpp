#include "BatteryOld.h"
#include <QHBoxLayout>
#include <QSvgRenderer>
#include <QPainter>
#include <QImage>
#include "DBusObject.h"

BatteryWidget::BatteryWidget(QWidget* parent, const QSize& bat, const QSize& bolt)
    : QWidget(parent)
{
    int maxW = bat.width() + bolt.width();
    int biggerH = bat.height() >= bolt.height() ? bat.height() : bolt.height();
    setFixedSize(maxW + 8, biggerH);

    loadIcons(bat, bolt);

    const QString uPowerIFace = "org.freedesktop.UPower";
    const QString uPowerPath = "/org/freedesktop/UPower/devices";
    const QString uPowerDevice = uPowerIFace + ".Device";

    DBusObject* battery = new DBusObject(
        uPowerIFace,
        uPowerPath + "/battery_BAT0",
        uPowerDevice,
        true,
        this
    );
    DBusObject* charger = new DBusObject(
        uPowerIFace,
        uPowerPath + "/line_power_AC",
        uPowerDevice,
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

    // Query properties to init widget on creation
    onChargeChanged(battery->property<int>("Percentage"));
    onPluggedIn(charger->property<bool>("Online"));
}


void BatteryWidget::onPluggedIn(bool is) {
    m_isPluggedIn = is;
    update();
}


void BatteryWidget::onChargeChanged(int charge) {
    if (charge >= 90) {
        m_state = Full;
    }
    else if (charge >= 75) {
        m_state = ThreeQuarters;
    }
    else if (charge >= 50) {
        m_state = Half;
    }
    else if (charge >= 25) {
        m_state = Quarter;
    }
    else {
        m_state = Low;
    }
    m_charge = charge;
    update();
}


void BatteryWidget::handleProperties(const QString& path, const QVariantMap& changed, const QStringList& invalidated) {
    Q_UNUSED(path);
    Q_UNUSED(invalidated);

    if (changed.contains("Percentage")) {
        onChargeChanged(changed["Percentage"].toInt());
    }

    if (changed.contains("Online")) {
        onPluggedIn(changed["Online"].toBool());
    }
}


QPixmap BatteryWidget::svgToPixmap(const QString& path, const QSize& size) {
    QSvgRenderer renderer(path);

    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);
 
    QPainter painter(&pixmap);
    renderer.render(&painter);

    return pixmap;
}


void BatteryWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);

    if (m_mouseOnWidget) {
        painter.drawText(this->rect(), Qt::AlignRight | Qt::AlignVCenter, (QString::number(m_charge) + "%"));
    }
    else {
        painter.drawPixmap(m_icons[PluggedIn].width(), 0, m_icons[m_state]);

        if (m_isPluggedIn) {
            int h = (rect().height() - m_icons[PluggedIn].height()) / 2;
            painter.drawPixmap(0, h, m_icons[PluggedIn]);
        };
    }
}


void BatteryWidget::enterEvent(QEnterEvent* event) {
    QWidget::enterEvent(event);
    m_mouseOnWidget = true;
    update();
}

void BatteryWidget::leaveEvent(QEvent* event) {
    QWidget::leaveEvent(event);
    m_mouseOnWidget = false;
    update();
}

QPixmap BatteryWidget::pixmapChangeColor(const QPixmap& pixmap, const QColor& color) {
    QImage img = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);

    for (int y = 0; y < img.height(); y++) {
        for (int x = 0; x < img.width(); x++) {
            QColor orginal = img.pixelColor(x, y);
            int alpha = orginal.alpha();
            if (alpha > 0) {
                QColor newColor = color;
                newColor.setAlpha(alpha);
                img.setPixelColor(x, y, newColor);
            }
        }
    }
    return QPixmap::fromImage(img);
}


void BatteryWidget::loadIcons(const QSize& battery, const QSize& bolt) {
    m_icons[Low] = svgToPixmap(":resources/battery-empty.svg", battery);
    m_icons[Quarter] = svgToPixmap(":resources/battery-quarter.svg", battery);
    m_icons[Half] = svgToPixmap(":resources/battery-half.svg", battery);
    m_icons[ThreeQuarters] = svgToPixmap(":resources/battery-three-quarters.svg", battery);
    m_icons[Full] = svgToPixmap(":resources/battery-full.svg", battery);
    m_icons[PluggedIn] = svgToPixmap(":resources/bolt.svg", bolt);
}
